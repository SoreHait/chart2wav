from pydub import AudioSegment
import sys

def music_mix(m1_path: str, m2_path: str, out_path: str, offset=0):
    _r = m1_path.split(".")
    m1_type = _r[-1]
    _r = m2_path.split(".")
    m2_type = _r[-1]
    _r = out_path.split(".")
    out_type = _r[-1]

    m1 = AudioSegment.from_file(m1_path, format=m1_type)
    m2 = AudioSegment.from_file(m2_path, format=m2_type)

    m1 = m1.apply_gain(-3)
    m2 = m2.apply_gain(+3)

    export = m1.overlay(m2, position=offset)
    export.export(out_path, format=out_type)
    print(f'File written to "{out_path}".')


if __name__ == "__main__":
    args = sys.argv

    if len(args) == 4:
        music_mix(args[1], args[2], args[3])

    elif len(args) == 5:
        music_mix(args[1], args[2], args[3], int(args[4]))

    else:
        print('Not enough or too much args.')
