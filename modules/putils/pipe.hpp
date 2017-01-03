
namespace putils {

class pipe {
public:

  int read_fd(){return read_fd_;}
  int write_fd(){return write_fd_;}

  pipe(): read_fd_(-1), write_fd_(-1){}

  pipe(const pipe& other): read_fd_(-1), write_fd_(-1) {
    (*this) = other;
  }

  pipe& operator=(const pipe& other) {
    if (other.read_fd_ != -1) {
      read_fd_ = ::dup(other.read_fd_);
    }
    if (other.write_fd_ != -1) {
      write_fd_ = ::dup(other.write_fd_);
    }
    return *this;
  }

  pipe(pipe&& other): read_fd_(-1), write_fd_(-1) {
    (*this) = other;
  }

  pipe& operator=(pipe& other) {
    read_fd_ = other.read_fd_;
    other.read_fd_ = -1;

    write_fd_ = other.write_fd_;
    other.write_fd_ = -1;
    return *this;
  }

  ~pipe() {
    if (read_fd_ != -1) {
      close(read_fd_);
    }

    if (write_fd_ != -1) {
      close(write_fd_);
    }
  }

private:
  pipe(int read, int write): read_fd_(read), write_fd_(write) { }


  int read_fd_;
  int write_fd_;

  friend pipe make_pipe();
};

pipe make_pipe() {
  int ends[2];
  ::pipe(ends);
  return pipe(ends[0], ends[1]);
}

}
