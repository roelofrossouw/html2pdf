# html2pdf

HTML files as PDFs, from the command line: pages with a header and footer,
page numbers, a table of contents, links and bookmarks, through
[sc-html2pdf](https://github.com/roelofrossouw/simply-cpp-html2pdf) (litehtml
and PDFHummus).

```bash
html2pdf invoice.html                                   # writes invoice.pdf next to it
html2pdf report.html report.pdf -H header.html -f footer.html -c site.css --toc 2
html2pdf card.html --landscape -s 10 -l 10 -u https://www.example.com
html2pdf statement.html -w open -p owner                # passwords: readers may only print
```

`html2pdf --help` lists the options: paper (`--size`, `--landscape`), margins
in mm (`--start` for the first page's top, default 60; `--top` 10, `--right`
10, `--bottom` 10, `--left` 30), `--header`, `--footer` and `--css` files,
`--toc PAGE`, `--base-url` (default: the input's folder), `--title`,
`--author`, `--password`, `--user-password`, `--builtin-fonts` (PDF's built-in Helvetica, Times
and Courier, nothing embedded: smaller files) and `--draw-margins`.

In the header and footer `#Page#`, `#ToPage#`, `#section#`, `#subsection#`
and `#subtitle#` are filled in per page; in a header `#P1Head` shows on the
first page only and `#POHead` on the others. In the content `<section>` starts
a page and an entry in the table of contents.

## Building

`bash scripts/install.sh` builds, tests, packages and installs it (to
`/usr/local/bin`). It needs sc-html2pdf.

```bash
sudo curl -fsSL https://apt.roelof.co.za/setup.sh | bash
sudo apt -y install html2pdf
```
