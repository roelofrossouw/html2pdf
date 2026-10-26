# NAME
html2pdf - HTML files as PDFs, with headers, footers, page numbers and a table of contents

# DESCRIPTION
A new page starts at every `<section>`, and at an element styled `page-break-before: always`;
`page-break-inside: avoid` keeps an element on one page when it fits on one.

In a header or footer, `#Page#` and `#ToPage#` become the page's number and the last page's;
`#section#`, `#subsection#` and `#subtitle#` the text of the last such element on or before the
page. In a header, an element with `id="P1Head"` shows on the first page only, and one with
`id="POHead"` on the others.

Lengths in the HTML are CSS pixels, 96 to the inch. Fonts are found by fontconfig and embedded,
unless **--builtin-fonts** is given.

# EXIT STATUS
0 when the PDF was written, 1 when it couldn't be (a file that can't be read or written), and 2
for a mistake in the options.

# EXAMPLES
A file, as a PDF next to it:
```
html2pdf invoice.html
```
A web page, its images and stylesheets found where it came from:
```
curl -fsSL https://www.google.com | html2pdf -u https://www.google.com google.pdf
```
A report with a header, footer, stylesheet and a table of contents on page 2:
```
html2pdf report.html report.pdf -H header.html -f footer.html -c site.css --toc 2
```
A statement readers need a password to open, and may only print:
```
html2pdf statement.html -w open -p owner
```
From stdin to stdout, in the middle of a pipe:
```
generate-report | html2pdf - - > report.pdf
```

# AUTHORS
Roelof Rossouw, as part of simply-cpp. The HTML and CSS are laid out by litehtml, by Yuri Kobets
and its contributors (https://github.com/litehtml/litehtml); the PDF is written by simply-cpp's own
PDF engine.

# SEE ALSO
The guide, with more examples: https://github.com/roelofrossouw/simply-cpp/wiki/HTML-to-PDF

html2pdf-php(7), the same from PHP: https://github.com/roelofrossouw/html2pdf-php
