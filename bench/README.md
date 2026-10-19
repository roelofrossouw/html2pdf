# Benchmark

Seven documents, each adding to the one before, run through html2pdf (fonts embedded, and
`--builtin-fonts`), wkhtmltopdf and xhtml2pdf as Ubuntu packages them. The results are on the wiki's
[Why simply-cpp](https://github.com/roelofrossouw/simply-cpp/wiki/Why-simply-cpp) page.

1. Hello, world.
2. A longer document with big type, cut across pages.
3. Tables, styles and colours.
4. Relative and absolute positions, a PNG and an SVG.
5. Sections and TrueType fonts (`@font-face`, DejaVu).
6. Headers and footers.
7. 220 pages and more.

```bash
python3 make.py          # writes the documents and their images here
./run.sh                 # needs html2pdf, wkhtmltopdf, xhtml2pdf3, poppler-utils and GNU time
```
