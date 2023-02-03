#!/usr/bin/env python3

"""This program calculates x,y coordinates for 90 angle arcs at various
  distances from a point.

  The format is included in the generated include file for clarity.
"""

import math
from typing import TextIO

DISTANCE_SCALE = 0.2
MIN_WEIGHT = 0x10
ANGLE_INCREMENT = math.pi / 1800

def get_weight(distance: int) -> int:
  if distance == 0:
    return 255
  cir = 2.0 * math.pi * float(distance) * DISTANCE_SCALE
  return round(255.0 / cir)


def calc_weights() -> list[int]:
  weights = []
  distance = 0
  while True:
    w = get_weight(distance)
    if w < MIN_WEIGHT:
      return weights
    weights.append(w)
    distance = distance + 1.0

def gen_points(distance: int) -> list[tuple[int, int]]:
  """Format is (x, y)"""
  data = []
  angle = 0.0
  while True:
    x = round(distance * math.cos(angle))
    y = round(distance * math.sin(angle))
    if not data or data[-1] != (x, y):
      data.append((x, y))
    angle = angle + ANGLE_INCREMENT
    if angle > math.pi / 2.0:
      break
  return data

def generate_header() -> None:
  output = (
      '// This file was generated by running matrix_clock/tools/distance_generator.py',
      '// with no arguments and redirecting the output to a file.',
      '',
  )
  print('\n'.join(output))

def generate_weights(weights: list[int]) -> None:
  data = (
      f'#define NUM_DISTANCES {len(weights)}  // MAX is -1 this val',
      '',
      '// Each number below is the weight to apply to a color at a given ',
      '// distance.  The range is 0x00-0xFF.',
      '',
      'uint8_t distance_weights[NUM_DISTANCES] __in_flash() = {',
  )
  print('\n'.join(data))
  for idx, w in enumerate(weights):
    print('    0x%02X,  // %d' % (w, idx))
  print('};\n')

def generate_offsets(distances: list[list[tuple[int, int]]]) -> None:
  data = (
      '',
      '// Each number below is the offset into the array for the given ',
      '// distance.  The next element is used to calculate the length.',
      '// The final element is used only for the length calculation',
      '',
      'uint16_t distance_offsets[NUM_DISTANCES + 1] __in_flash() = {',
  )
  print('\n'.join(data))
  offset = 0;
  for idx, point_list in enumerate(distances):
    print(f'    0x%04X,  // %d' % (offset, idx))
    offset += len(point_list)
  print(f'    0x%04X,' % offset)
  print('};\n')

def chunks(lst, n):
  """Yield successive n-sized chunks from lst."""
  for i in range(0, len(lst), n):
    yield lst[i:i + n]

def generate_point_list(idx: int, points: list[tuple[int, int]]) -> None:
  print(f'    // {idx}')
  for pl in chunks(points, 8):
    line = ['   ']
    for p in pl:
      x, y = p
      line.append('0x%02X%02X,' % (x, y))
    print(' '.join(line))

def generate_data(distances: list[list[tuple[int, int]]]) -> None:
  num_points = sum(len(pl) for pl in distances)
  data = (
      '// each element is of the format 0xXXYY',
      'uint16_t distance_points[%u] __in_flash() = {' % num_points,
  )
  print('\n'.join(data))
  for idx, point_list in enumerate(distances):
    generate_point_list(idx, point_list)
  print('};\n')


def generate_output(
  weights: list[int], distances: list[list[tuple[int, int]]]) -> None:
  generate_header()
  generate_weights(weights)
  generate_offsets(distances)
  generate_data(distances)

def main() -> None:
  weights = calc_weights()
  distances = [gen_points(d) for d in range(len(weights))]
  generate_output(weights, distances)

if __name__ == '__main__':
  main()
