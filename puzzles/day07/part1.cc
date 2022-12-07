#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

class Directory;
class File;

class FsNode {
 public:
  virtual size_t ByteSize() const = 0;

  absl::string_view Name() const { return name_; }

  Directory* Parent() const { return parent_; }

 protected:
  explicit FsNode(std::string name, Directory* parent)
      : name_(std::move(name)), parent_(parent) {}

 private:
  std::string name_;
  Directory* parent_ = nullptr;
};

class Directory final : public FsNode {
 public:
  explicit Directory(std::string name, Directory* parent)
      : FsNode(std::move(name), parent) {}

  size_t ByteSize() const override;

  void AddFile(std::unique_ptr<File> child) {
    files_.emplace_back(std::move(child));
  }

  void AddDir(std::unique_ptr<Directory> child) {
    dirs_.emplace_back(std::move(child));
  }

  Directory* SubDir(absl::string_view name) const {
    for (const std::unique_ptr<Directory>& dir : dirs_) {
      if (dir->Name() == name) return dir.get();
    }
    return nullptr;
  }

  size_t TotalSizeAtMost100K() const {
    size_t total = 0;
    if (ByteSize() <= 100000) {
      total += ByteSize();
    }
    for (const std::unique_ptr<Directory>& child : dirs_) {
      total += child->TotalSizeAtMost100K();
    }
    return total;
  }

 private:
  std::vector<std::unique_ptr<File>> files_;
  std::vector<std::unique_ptr<Directory>> dirs_;
  mutable std::optional<size_t> cached_size_;
};

class File final : public FsNode {
 public:
  explicit File(std::string name, size_t byte_size, Directory* parent)
      : FsNode(std::move(name), parent), byte_size_(byte_size) {}

  size_t ByteSize() const override { return byte_size_; }

 private:
  size_t byte_size_;
};

Directory BuildTree(const std::vector<std::string>& shell_session) {
  Directory root("/", nullptr);
  Directory* current = &root;
  bool in_listing = false;
  for (absl::string_view line : shell_session) {
    if (line.front() == '$') {
      in_listing = false;
      if (line.substr(2, 2) == "cd") {
        const absl::string_view target = line.substr(5);
        if (target == "/") {
          current = &root;
          continue;
        }
        if (target == "..") {
          current = current->Parent();
          CHECK_NE(current, nullptr);
          continue;
        }
        current = current->SubDir(target);
        CHECK_NE(current, nullptr);
        continue;
      }
      CHECK_EQ(line.substr(2), "ls");
      in_listing = true;
      continue;
    }
    CHECK(in_listing);
    std::vector<absl::string_view> parts = absl::StrSplit(line, ' ');
    CHECK_EQ(2, parts.size());
    if (parts.front() == "dir") {
      current->AddDir(
          std::make_unique<Directory>(std::string(parts.back()), current));
      continue;
    }
    size_t filesize = 0;
    CHECK(absl::SimpleAtoi(parts.front(), &filesize));
    current->AddFile(
        std::make_unique<File>(std::string(parts.back()), filesize, current));
    continue;
  }
  return root;
}

size_t Directory::ByteSize() const {
  if (cached_size_.has_value()) return *cached_size_;
  size_t total = 0;
  for (const std::unique_ptr<File>& child : files_) {
    total += child->ByteSize();
  }
  for (const std::unique_ptr<Directory>& child : dirs_) {
    total += child->ByteSize();
  }
  cached_size_ = total;
  return total;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Directory root = BuildTree(lines);
  std::cout << root.TotalSizeAtMost100K() << "\n";
  return 0;
}
