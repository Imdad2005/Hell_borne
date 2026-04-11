from PIL import Image
import argparse
import glob
import os
import re
from pathlib import Path

# ── CONFIG ────────────────────────────────────────────────────────────────────
# Put this script in the same folder as your 8 frame images
# Name your frames:  run_1.jpg, run_2.jpg ... run_8.jpg  (or .png)

FRAME_FILES = [
    "ezgif-frame-001.png",
    "ezgif-frame-002.png",
    "ezgif-frame-003.png",
    "ezgif-frame-004.png",
    "ezgif-frame-005.png",
    "ezgif-frame-006.png",
    "ezgif-frame-007.png",
    "ezgif-frame-008.png",
]

OUTPUT_FILE   = "player_sheet_3.png"   # output sprite sheet name
FRAME_SIZE    = (96, 96)               # each frame will be resized to this (w x h)
BLACK_THRESH  = 30                     # 0-255, pixels darker than this become transparent
# ─────────────────────────────────────────────────────────────────────────────

DEFAULT_PATTERN = "ezgif-frame-*.*"


def remove_black_bg(img: Image.Image, threshold: int) -> Image.Image:
    """Convert near-black pixels to transparent."""
    img = img.convert("RGBA")
    pixels = img.load()
    width, height = img.size
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            if r < threshold and g < threshold and b < threshold:
                pixels[x, y] = (0, 0, 0, 0)   # fully transparent
    return img


def parse_size(text: str) -> tuple[int, int]:
    """Parse WxH text like 96x96 into an integer tuple."""
    m = re.fullmatch(r"\s*(\d+)\s*[xX]\s*(\d+)\s*", text)
    if not m:
        raise ValueError("Size must be in WxH format, for example: 96x96")
    w, h = int(m.group(1)), int(m.group(2))
    if w <= 0 or h <= 0:
        raise ValueError("Size values must be positive")
    return (w, h)


def natural_sort_key(path_text: str):
    """Sort frame names so frame-2 comes before frame-10."""
    parts = re.split(r"(\d+)", path_text)
    return [int(p) if p.isdigit() else p.lower() for p in parts]


def resolve_frame_paths(input_dir: Path, explicit_files: list[str], pattern: str) -> list[Path]:
    """Get frame file paths from explicit list, or fall back to glob pattern."""
    resolved: list[Path] = []

    for name in explicit_files:
        p = input_dir / name
        if p.exists() and p.is_file():
            resolved.append(p)

    if resolved:
        return resolved

    # No explicit list found: try glob-based discovery.
    matches = glob.glob(str(input_dir / pattern))
    file_paths = [Path(m) for m in matches if Path(m).is_file()]
    file_paths.sort(key=lambda p: natural_sort_key(str(p.name)))
    return file_paths


def build_spritesheet(
    frame_paths: list[Path],
    output_file: Path,
    frame_size: tuple[int, int],
    black_thresh: int,
    remove_black: bool,
) -> int:
    frames: list[Image.Image] = []

    for p in frame_paths:
        try:
            img = Image.open(p)
            img = img.resize(frame_size, Image.LANCZOS)
            if remove_black:
                img = remove_black_bg(img, black_thresh)
            else:
                img = img.convert("RGBA")
            frames.append(img)
            print(f"  [+] Processed: {p.name}")
        except Exception as exc:
            print(f"  [!] Failed to process {p.name}: {exc}")

    if not frames:
        print("No usable frames found.")
        return 1

    sheet_w = frame_size[0] * len(frames)
    sheet_h = frame_size[1]
    sheet = Image.new("RGBA", (sheet_w, sheet_h), (0, 0, 0, 0))

    for i, frame in enumerate(frames):
        sheet.paste(frame, (i * frame_size[0], 0), frame)

    output_file.parent.mkdir(parents=True, exist_ok=True)
    sheet.save(output_file)
    print(f"\nSaved sprite sheet: {output_file}  ({sheet_w}x{sheet_h})")
    return 0


def main():
    script_dir = Path(__file__).resolve().parent

    parser = argparse.ArgumentParser(
        description="Build a horizontal spritesheet from image frames.")
    parser.add_argument(
        "--input-dir",
        default=str(script_dir),
        help="Folder containing frame images (default: script folder)")
    parser.add_argument(
        "--output",
        default=OUTPUT_FILE,
        help="Output file name or path")
    parser.add_argument(
        "--size",
        default=f"{FRAME_SIZE[0]}x{FRAME_SIZE[1]}",
        help="Frame size in WxH, for example 96x96")
    parser.add_argument(
        "--threshold",
        type=int,
        default=BLACK_THRESH,
        help="Black background threshold (0-255)")
    parser.add_argument(
        "--pattern",
        default=DEFAULT_PATTERN,
        help="Glob pattern used when explicit frame list is missing")
    parser.add_argument(
        "--no-remove-black",
        action="store_true",
        help="Do not remove near-black background")
    parser.add_argument(
        "--frames",
        nargs="*",
        default=FRAME_FILES,
        help="Explicit frame filenames in order")

    args = parser.parse_args()

    try:
        frame_size = parse_size(args.size)
    except ValueError as exc:
        print(f"Invalid --size: {exc}")
        return 1

    threshold = max(0, min(255, int(args.threshold)))
    input_dir = Path(args.input_dir).expanduser().resolve()

    if not input_dir.exists() or not input_dir.is_dir():
        print(f"Input directory does not exist: {input_dir}")
        return 1

    frame_paths = resolve_frame_paths(input_dir, args.frames, args.pattern)
    if not frame_paths:
        print("No frames found.")
        print(f"Checked explicit frame list and pattern '{args.pattern}' in: {input_dir}")
        return 1

    output_path = Path(args.output)
    if not output_path.is_absolute():
        output_path = input_dir / output_path

    print(f"Input dir:   {input_dir}")
    print(f"Frame count: {len(frame_paths)}")
    print(f"Frame size:  {frame_size[0]}x{frame_size[1]}")
    print(f"Output:      {output_path}")

    return build_spritesheet(
        frame_paths=frame_paths,
        output_file=output_path,
        frame_size=frame_size,
        black_thresh=threshold,
        remove_black=not args.no_remove_black,
    )


if __name__ == "__main__":
    raise SystemExit(main())
