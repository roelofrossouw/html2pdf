#!/usr/bin/env python3
# The seven benchmark documents, each adding features to the one before.
import base64, random, struct, zlib
random.seed(7)
W = "lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua enim ad minim veniam quis nostrud exercitation ullamco laboris nisi aliquip ex ea commodo consequat".split()
def sentence(n=None):
    n = n or random.randint(8, 18); s = " ".join(random.choice(W) for _ in range(n)); return s[0].upper() + s[1:] + "."
def para(n=None): return "<p>" + " ".join(sentence() for _ in range(n or random.randint(3, 6))) + "</p>"

def png(w, h):  # a small gradient PNG
    raw = b"".join(b"\0" + bytes(c for x in range(w) for c in (x * 255 // w, y * 255 // h, 160)) for y in range(h))
    chunk = lambda t, d: struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d))
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")
open("chart.png", "wb").write(png(240, 120))
open("logo.svg", "w").write('<svg xmlns="http://www.w3.org/2000/svg" width="120" height="60" viewBox="0 0 120 60"><rect x="2" y="2" width="116" height="56" rx="10" fill="#1d4e89" stroke="#0b2545" stroke-width="3"/><circle cx="30" cy="30" r="16" fill="#f4a259"/><path d="M55 40 L70 18 L85 40 Z" fill="#8cb369"/><rect x="92" y="18" width="18" height="22" fill="#e9eff6"/></svg>')

def doc(body, css="", title="Benchmark"):
    return f"<!DOCTYPE html><html><head><meta charset='utf-8'><title>{title}</title><style>{css}</style></head><body>{body}</body></html>"

BASE = "body { font-family: Helvetica, Arial, sans-serif; font-size: 10pt; } @page { size: A4; margin: 10mm; }"
# 1. hello world
open("1-hello.html", "w").write(doc("<h1>Hello, world</h1><p>A first PDF.</p>", BASE))
# 2. longer, with big type that has to break across pages
def long_text(parts=12):
    out = ""
    for i in range(parts):
        out += f"<h1 style='font-size:36pt'>Chapter {i + 1}</h1>" + "".join(para() for _ in range(4))
        out += f"<p style='font-size:28pt;line-height:1.2'>{sentence(30)}</p>" + "".join(para() for _ in range(3))
    return out
open("2-long.html", "w").write(doc(long_text(), BASE))
# 3. + tables, styles and colours
CSS3 = BASE + """
h1 { color: #1d4e89; border-bottom: 2px solid #f4a259; padding-bottom: 4px; }
.note { background: #fdf3e7; border-left: 4px solid #f4a259; padding: 6px 10px; margin: 8px 0; border-radius: 4px; }
table { width: 100%; border-collapse: collapse; margin: 8px 0; }
th { background: #1d4e89; color: white; text-align: left; padding: 4px; }
td { padding: 4px; border-bottom: 1px solid #ccd; }
tr:nth-child(even) td { background: #eef3f8; }
td.amount { text-align: right; font-family: Courier, monospace; }
em { color: #8b1e3f; } strong { color: #0b2545; }"""
def table(rows=12):
    t = "<table><tr><th>Item</th><th>Description</th><th>Qty</th><th>Amount</th></tr>"
    for r in range(rows):
        t += f"<tr><td>{r + 1:03}</td><td>{sentence(6)}</td><td>{random.randint(1, 20)}</td><td class=amount>{random.randint(100, 99999) / 100:.2f}</td></tr>"
    return t + "</table>"
def styled(parts=12, extra=lambda i: ""):
    out = ""
    for i in range(parts):
        out += f"<h1>Chapter {i + 1}</h1>" + para() + f"<div class=note><strong>Note.</strong> <em>{sentence()}</em></div>" + para()
        out += table() + extra(i) + f"<p style='font-size:24pt;color:#8cb369'>{sentence(20)}</p>" + para()
    return out
open("3-styled.html", "w").write(doc(styled(), CSS3))
# 4. + relative positions, SVG and PNG images
CSS4 = CSS3 + """
.figure { position: relative; height: 140px; margin: 10px 0; border: 1px solid #ccd; border-radius: 6px; }
.figure img.chart { position: absolute; left: 10px; top: 10px; }
.figure img.logo { position: absolute; right: 12px; top: 40px; }
.figure .badge { position: relative; top: 100px; left: 270px; background: #8b1e3f; color: white; padding: 2px 8px; border-radius: 8px; }"""
fig = lambda i: f"<div class=figure><img class=chart src='chart.png' width=240 height=120><img class=logo src='logo.svg' width=120 height=60><span class=badge>Figure {i + 1}</span></div>"
open("4-images.html", "w").write(doc(styled(extra=fig), CSS4))
# 5. + sections and TrueType fonts (DejaVu, as every Ubuntu has it)
CSS5 = CSS4 + """
@font-face { font-family: 'Body Serif'; src: url('file:///usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf'); }
@font-face { font-family: 'Body Mono'; src: url('file:///usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf'); }
p { font-family: 'Body Serif', 'DejaVu Serif', serif; }
td.amount { font-family: 'Body Mono', 'DejaVu Sans Mono', monospace; }
section { page-break-before: always; font-size: 18pt; color: #0b2545; }"""
def sectioned(parts=12):
    random.seed(7); body = styled(1, fig)  # same start
    out = ""
    for i in range(parts):
        out += f"<section>Part {i + 1}</section>" + styled(1, fig).replace("Chapter 1", f"Chapter {i + 1}")
    return out
open("5-sections.html", "w").write(doc(sectioned(), CSS5))
# 6. + headers and footers: html2pdf's own files; xhtml2pdf's @page frames
open("6-header.html", "w").write("<div style='font-size:8pt;color:#666;border-bottom:1px solid #ccc'>Benchmark report &mdash; #section#</div>")
open("6-footer.html", "w").write("<div style='font-size:8pt;color:#666;text-align:right'>Page #Page# of #ToPage#</div>")
open("6-header-wk.html", "w").write("<!DOCTYPE html><html><body style='font-size:8pt;color:#666;border-bottom:1px solid #ccc;font-family:Helvetica'>Benchmark report</body></html>")
open("6-footer-wk.html", "w").write("<!DOCTYPE html><html><head><script>function s(){var v={};location.search.substring(1).split('&').forEach(function(p){var z=p.split('=');v[z[0]]=decodeURIComponent(z[1]);});document.getElementById('p').textContent=v.page;document.getElementById('t').textContent=v.topage;}</script></head><body onload='s()' style='font-size:8pt;color:#666;text-align:right;font-family:Helvetica'>Page <span id=p></span> of <span id=t></span></body></html>")
X6 = """@page { size: A4; margin: 10mm; margin-top: 20mm; margin-bottom: 20mm;
  @frame header { -pdf-frame-content: hdr; top: 8mm; left: 10mm; right: 10mm; height: 8mm; }
  @frame footer { -pdf-frame-content: ftr; bottom: 8mm; left: 10mm; right: 10mm; height: 8mm; } }"""
body5 = sectioned()
open("6-headers.html", "w").write(doc(body5, CSS5))
open("6-headers-x.html", "w").write(doc("<div id=hdr style='font-size:8pt;color:#666'>Benchmark report</div><div id=ftr style='font-size:8pt;color:#666;text-align:right'>Page <pdf:pagenumber> of <pdf:pagecount></div>" + body5, CSS5 + X6))
# 7. 200+ pages: part 6 many times over
big = "".join(sectioned() for _ in range(11))  # 220+ pages
open("7-large.html", "w").write(doc(big, CSS5))
open("7-large-x.html", "w").write(doc("<div id=hdr style='font-size:8pt;color:#666'>Benchmark report</div><div id=ftr style='font-size:8pt;color:#666;text-align:right'>Page <pdf:pagenumber> of <pdf:pagecount></div>" + big, CSS5 + X6))
print("written")
