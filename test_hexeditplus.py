import unittest
import subprocess
import os

class TestHexeditPlus(unittest.TestCase):
    executable = "./hexeditplus"
    source_file = "task1.c"
    test_file = "test_data.bin"

    @classmethod
    def setUpClass(cls):
        compile_cmd = ["gcc", "-m32", "-g", "-Wall", "-o", "hexeditplus", cls.source_file]
        subprocess.run(compile_cmd, check=True)

    def setUp(self):
        with open(self.test_file, "wb") as f:
            f.write(bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]))

    def tearDown(self):
        if os.path.exists(self.test_file):
            os.remove(self.test_file)

    @classmethod
    def tearDownClass(cls):
        if os.path.exists(cls.executable):
            os.remove(cls.executable)

    def run_app(self, inputs):
        input_string = "\n".join(inputs) + "\n"
        process = subprocess.run(
            [self.executable],
            input=input_string,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True
        )
        return process.stdout, process.stderr

    def test_quit(self):
        out, err = self.run_app(["Q"])
        self.assertIn("Choose action:", out)

    def test_toggle_debug(self):
        out, err = self.run_app(["D", "Q"])
        self.assertIn("Debug flag now on", err)
        self.assertIn("quitting", err)

    def test_set_unit_size(self):
        out, err = self.run_app(["D", "U", "2", "Q"])
        self.assertIn("Debug: set size to 2", err)
        self.assertIn("unit_size: 2", err)

    def test_invalid_unit_size(self):
        out, err = self.run_app(["U", "3", "Q"])
        self.assertIn("Invalid unit size", out)

    def test_set_file_name(self):
        out, err = self.run_app(["D", "F", "dummy.txt", "Q"])
        self.assertIn("Debug: file name set to 'dummy.txt'", err)

    def test_load_into_memory(self):
        out, err = self.run_app(["F", self.test_file, "L", "0 4", "Q"])
        self.assertIn("Loaded 4 units into memory", out)

    def test_toggle_display_mode(self):
        out, err = self.run_app(["T", "T", "Q"])
        self.assertIn("Decimal display flag now on", out)
        self.assertIn("Decimal display flag now off", out)

    def test_memory_modify(self):
        out, err = self.run_app(["y", "0 FF", "D", "Q"])
        self.assertIn("Choose action:", out)

if __name__ == '__main__':
    unittest.main()
