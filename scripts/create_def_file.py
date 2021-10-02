#!/usr/bin/env python3
from argparse import ArgumentParser, FileType


def parse_args():
    parser = ArgumentParser("Receives an file and generates a C file that defines two variables - the content and length of the file.")
    parser.add_argument("var_name", type=str, help="The name of the variable defined in the C file.")
    parser.add_argument("input_file", type=FileType("rb"), help="The file whose content should be saved in the variables.")
    return parser.parse_args()


def format_array(data):
    result = ""
    for byte in data:
        result += "\\x{:02x}".format(byte)
    return result


def main():
    args = parse_args()

    content = args.input_file.read()
    length = len(content)
    name = args.var_name
    print(f"const char *{name} = \"{format_array(content)}\";\nconst unsigned int {name}_len = {length};")


if __name__ == "__main__":
    main()
