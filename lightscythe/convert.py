#!/usr/bin/python
__author__ = 'gfh'

import PIL
import Image
import sys
import math

RED     = 0b100
GREEN   = 0b001
BLUE    = 0b010

class ScytheConverter(object):
    def __init__(self, infn, rows):
        self.infn = infn
        fnsplit = infn.split('.')
        self.scyfn = str.join('.', fnsplit[:-1] + ['scy'])
        self.cnvfn = str.join('.', fnsplit[:-2] + [fnsplit[-2] + '_scy'] + ['png'])
        self.rows = rows

    def convert(self):
        self.load_image()
        self.convert_image()

    def load_image(self):
        self.image = Image.open(self.infn)
        width, height = self.image.size
        self.image = self.image.convert("RGB")
        new_width = int(float(width) * self.rows / height)
        dem = (new_width, self.rows)
        print "Original image: ", self.image.size
        self.image = self.image.resize(dem)
        print "Scaling to: ", self.image.size

    def convert_image(self):
        data = ''
        datum = None
        width, height = self.image.size
        cnv = Image.new("RGB", self.image.size)
        for x in range(width):
            for y in range(height):
                xy = (x, y)
                rgb = self.image.getpixel(xy)
                rgb = self.color_match(rgb)
                cnv.putpixel(xy, self.rgb_value(rgb))
                data += chr(rgb)
        out = open(self.scyfn, 'w')
        out.write(data)
        cnv.save(self.cnvfn)
        
    def rgb_value(self, idx):
        red = bool(idx & RED) * 0xff
        green = bool(idx & GREEN) * 0xff
        blue = bool(idx & BLUE) * 0xff
        return (red, green, blue)

    def color_match(self, rgb):
        sfun = lambda x, y: cmp(x[1], y[1])
        distances = []
        for idx in range(7):
            red, green, blue = self.rgb_value(idx)
            dist = (rgb[0] - red) ** 2
            dist += (rgb[1] - green) ** 2
            dist += (rgb[2] - blue) ** 2
            dist = math.sqrt(dist)
            dist = (idx, dist)
            distances.append(dist)
        distances.sort(sfun)
        return distances[0][0]

if __name__ == '__main__':
    infn = sys.argv[1]
    sc = ScytheConverter(infn, 70)
    sc.convert()
