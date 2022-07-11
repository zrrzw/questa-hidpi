#!/usr/bin/env python3

import argparse
import subprocess

from path import Path, TempDir
from PIL import Image, UnidentifiedImageError
from pytrofs.trofs import create, extract


def real_main(args):
    patch_file = (Path(__file__).parent / "hidpi.patch").normpath()
    # tmp_dir = TempDir()
    tmp_dir = Path("tmp")
    extract(args.in_file, tmp_dir)
    subprocess.check_call(["patch", "-p1"], stdin=open(patch_file), cwd=tmp_dir)
    for file in tmp_dir.walkfiles():
        try:
            img = Image.open(file)
            img = img.resize((img.width * 2, img.height * 2), resample=Image.Resampling.NEAREST)
            try:
                img.save(file)
            except KeyError:
                pass
        except UnidentifiedImageError:
            pass
        except ValueError:
            pass
    create(args.out_file, tmp_dir)


def main():
    parser = argparse.ArgumentParser(description="questa_hidpi.py")
    parser.add_argument("-i", "--in", dest="in_file", required=True, help="trofs input archive.")
    parser.add_argument("-o", "--out", dest="out_file", required=True, help="trofs output archive.")
    args = parser.parse_args()
    real_main(args)


if __name__ == "__main__":
    main()
