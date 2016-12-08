
import os
import unittest
import urllib2
import subprocess
import sys

class Server(object):
    def __init__(self):
        (r, w) = os.pipe()
        rfd = os.fdopen(r, "r")
        self._r = r
        self._w = w
        self._rfd = rfd
        self._process = subprocess.Popen(["./test"], stdout=self._w)
        while True:
            line = self._rfd.readline()
            print "OUT:", line[:-1]
            if 'Listening' in line:
                self._url = line.strip().split(' ')[-1]
                break

    def get_url(self, endpoint):
        if endpoint[:1] != '/':
            endpoint = '/' + endpoint
        try:
            response = urllib2.urlopen(self._url + endpoint)
            return (response.getcode(), response.read())
        except urllib2.HTTPError as e:
            return (e.code, None)

    def quit(self):
        self.get_url("terminate")
        self._process.poll()
        while self._process.returncode is None:
            self._process.poll()

        os.close(self._w)

        while True:
            line = self._rfd.readline()
            if len(line) == 0:
                break
            print "OUT:", line[:-1]
        return self._process.returncode


class TestHTTPServer(unittest.TestCase):
    def test_basic(self):
        server = Server()
        def cleanup():
            self.assertEqual(server.quit(), 0)

        self.addCleanup(cleanup)
        self.assertEqual(server.get_url(""), (200, "Hello World"))
        self.assertEqual(server.get_url("file"), (200, "Hello world\nFrom a file\n"))
        self.assertEqual(server.get_url("notfound"), (404, None))



if __name__ == "__main__":
    print "Starting main"
    unittest.main()
