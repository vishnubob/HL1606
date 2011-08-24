#!/usr/bin/python
__author__ = 'gfh'

import os
import PIL
import Image
import sys
import math
import optparse

RED     = 0b100
GREEN   = 0b001
BLUE    = 0b010

Defaults = {
    "margin_top": 0,
    "margin_bottom": 0,
    "reverse": False,
    "rows": 70,
}

class ScytheConverter(object):
    def __init__(self, infn, outfn, rows, margin_top=0, margin_bottom=0, proof=False, reverse=False, **kw):
        self.infn = infn
        self.outfn = outfn
        self.rows = rows
        self.margin_top = margin_top
        self.margin_bottom = margin_bottom
        self.reverse = reverse
        self.proof = proof

    def convert(self):
        self.load_image()
        self.convert_image()

    def load_image(self):
        self.image = Image.open(self.infn)
        width, height = self.image.size
        self.image = self.image.convert("RGB")
        new_width = int(float(width) * self.rows / height)
        dem = (new_width, self.rows)
        print "Original image (%s): %s" % (self.infn, self.image.size)
        self.image = self.image.resize(dem)
        print "Converted image (%s): %s" % (self.outfn, self.image.size)

    def convert_image(self):
        data = ''
        datum = None
        width, actual_height = self.image.size
        height = actual_height + self.margin_top + self.margin_bottom
        cnv = Image.new("RGB", self.image.size)
        for x in range(width):
            for y in range(height):
                if self.reverse:
                    y = height - (y + 1)
                if self.margin_top and (y < self.margin_top):
                    rgb = 0
                elif self.margin_bottom and (y > (self.margin_top + self.actual_height)):
                    rgb = 0
                else:
                    xy = (x, y - self.margin_top)
                    rgb = self.image.getpixel(xy)
                    rgb = self.color_match(rgb)
                cnv.putpixel(xy, self.rgb_value(rgb))
                data += chr(rgb)
        out = open(self.outfn, 'w')
        out.write(data)
        if self.proof:
            fnsplit = self.outfn.split('.')
            cnvfn = str.join('.', fnsplit[:-2] + [fnsplit[-2] + '_scy'] + ['png'])
            cnv.save(cnvfn)
        
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

class BatchConverter(object):
    def __init__(self, opts):
        self.opts = opts
        self.rows = self.opts['rows']
        del self.opts['rows']

    def convert(self):
        fnlist = os.listdir(self.opts["srcdir"])
        imgcnt = 0
        for fn in fnlist:
            infn = os.path.join(self.opts["srcdir"], fn)
            outfn = "%d.scy" % imgcnt
            outfn = os.path.join(self.opts["targetdir"], outfn)
            sc = ScytheConverter(infn, outfn, self.rows, **opts)
            sc.convert()
            imgcnt += 1
        datfn = "imgcnt.dat"
        datfn = os.path.join(self.opts["targetdir"], datfn)
        dat = str.join('', map(chr, [(imgcnt >> 8) & 0xff, (imgcnt & 0xff)]))
        f = open(datfn, 'w')
        f.write(dat)

def get_cli():
    parser = optparse.OptionParser()
    parser.add_option("-t", "--targetdir", dest="targetdir", help="Name of target directory")
    parser.add_option("-r", "--rows", dest="rows", type="int", help="Number of LEDs in a row")
    parser.add_option("-T", "--top", dest="margin_top", type="int", help="Size of top margin")
    parser.add_option("-B", "--bottom", dest="margin_bottom", type="int", help="Size of bottom margin")
    parser.add_option("-R", "--reverse", dest="reverse", action="store_true", help="Name of target directory")
    parser.set_defaults(**Defaults)
    (opts, args) = parser.parse_args()
    opts = eval(str(opts))
    if not args:
        print "Error: must provide a source directory"
        sys.exit(-1)
    opts['srcdir'] = args[0]
    if not os.path.exists(opts["srcdir"]):
        print "Error: directory '%s' does not exist!" % opts['srcdir']
        sys.exit(-1)
    if opts["targetdir"] == None:
        opts["targetdir"] = opts["srcdir"] + "_scy"
    if not os.path.isdir(opts["targetdir"]):
        os.mkdir(opts["targetdir"])
    return (opts, args)

if __name__ == '__main__':
    (opts, args) = get_cli()
    bc = BatchConverter(opts)
    bc.convert()
