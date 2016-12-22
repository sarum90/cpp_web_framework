#pragma once

#include "net.hpp"

class buffer {
public:
  buffer(int size): bytes_(size), chars_(std::make_unique<char[]>(size)) {}

  buffer(const buffer& other) = delete;
  buffer& operator=(const buffer& other) = delete;

  buffer(buffer&& other) noexcept = default;
  buffer& operator=(buffer&& other) = default;

  int size() { return bytes_; }

  char * get() { return &chars_[0]; }

private:
  int bytes_;
  std::unique_ptr<char[]> chars_;
};

template <class R>
class buffered_reader {
public:
  buffered_reader(reactor * rr, R* r): r_(rr), reader_(r) {}

  buffered_reader(const buffered_reader& br) = delete;
  buffered_reader& operator=(const buffered_reader& br) = delete;

  buffered_reader(buffered_reader&& br) = default;
  buffered_reader& operator=(buffered_reader&& br) = default;

  future<mes::mestring> read_some() {
    if(read_buffer_index_ + 1 < buffers_.size()) {
      int start = read_index_;
      auto& read_buffer = buffers_[read_buffer_index_];
      read_index_ = 0;
      read_buffer_index_++;
      return r_->make_ready_future<mes::mestring>(
          mes::make_mestring(read_buffer.get()+start, read_buffer.size()-start)
        );
    }
    if(read_buffer_index_ + 1 == buffers_.size() && read_index_ < written_index_) {
      int start = read_index_;
      auto& read_buffer = buffers_[read_buffer_index_];
      read_index_ = written_index_;
      return r_->make_ready_future<mes::mestring>(
          mes::make_mestring(read_buffer.get()+start, read_index_-start)
      );
    }
    auto ms = get_free_buffer();
    read_buffer_index_ = buffers_.size()-1;
    read_index_ = written_index_;
    return reader_->read(ms, remaining_bytes())
    .then([ms=ms, &wi=written_index_, &ri=read_index_](std::size_t sz) {
      wi += sz;
      ri += sz;
      return mes::make_mestring(&ms[0], sz);
    });
  }

  void replace_bytes(int bytes) {
    while (read_index_ < bytes) {
      bytes -= read_index_;
      read_buffer_index_--;
      if (read_buffer_index_ < 0 ) {
        throw std::runtime_error("Replaced more bytes than have been read.");
      }
      read_index_ = buffers_[read_buffer_index_].size();
    }
    read_index_ -= bytes;
  }


private:
  mes::mestring buffered() {
    const char * c = buffers_[read_buffer_index_].get() + read_index_;
    int len = buffers_[read_buffer_index_].size() - read_buffer_index_;
    if (read_buffer_index_ == buffers_.size() - 1) {
      len = written_index_ - read_buffer_index_;
    }
    return mes::make_mestring(c, len);
  }

  char * get_free_buffer() {
    if (remaining_bytes() == 0) {
      buffers_.push_back(buffer(4096));
      current_buffer_size_ = 4096;
      written_index_ = 0;
    }
    return buffers_.back().get() + written_index_;
  }

  std::size_t remaining_bytes() const {
    return current_buffer_size_ - written_index_;
  }

  R * reader_;
  reactor * r_;

  std::deque<buffer> buffers_;
  int current_buffer_size_ = 0;
  int written_index_ = 0;

  int read_buffer_index_ = 0;
  int read_index_ = 0;
};

template<class R>
inline buffered_reader<R> make_buffered_reader(reactor * rr, R* r) {
  return buffered_reader<R>(rr, r);
}
