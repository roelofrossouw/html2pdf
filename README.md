# html2pdf

Turn HTML into a PDF from the command line. Point it at a file and you get a
PDF next to it, with pages broken between lines of text rather than through
them. Want more? Add a header and footer with page numbers, a table of
contents, bookmarks, working links, your own fonts, or a password.

There's no browser behind it. The HTML and CSS are read and laid out by
[litehtml](https://github.com/litehtml/litehtml), by Yuri Kobets
([tordex](https://github.com/tordex)), and the PDF is written by
[simply-cpp](https://github.com/roelofrossouw/simply-cpp)'s own PDF engine,
written from scratch in C++, as a standards-compliant PDF (ISO 32000-1, PDF
1.7).

```bash
html2pdf invoice.html                     # writes invoice.pdf next to it
```

Got a web page instead of a file? Pipe it in:

```bash
curl -fsSL https://www.google.com | html2pdf -u https://www.google.com google.pdf
```

`-u` (`--base-url`) tells html2pdf where the page came from, so its images and
stylesheets can be found. Leave it out and you still get the text and layout,
just without the pictures.

The full guide, with more examples, is on the wiki:
**[HTML to PDF](https://github.com/roelofrossouw/simply-cpp/wiki/HTML-to-PDF)**.

## A few more things it can do

```bash
# a report with a header, a footer, a stylesheet and a table of contents on page 2
html2pdf report.html report.pdf -H header.html -f footer.html -c site.css --toc 2

# a card in landscape with smaller margins, its images found on a website
html2pdf card.html --landscape -s 10 -l 10 -u https://www.example.com

# a statement readers need a password to open, and may only print
html2pdf statement.html -w open -p owner

# PDF's own Helvetica, Times and Courier: nothing embedded, smaller files
html2pdf letter.html --builtin-fonts

# from stdin to stdout, for the middle of a pipe
generate-report | html2pdf - - > report.pdf
```

In a header or footer, `#Page#` and `#ToPage#` become "page 3 of 7", and
`#section#`, `#subsection#` and `#subtitle#` show where the reader is. In a
header, `#P1Head` shows on the first page only and `#POHead` on all the others.
In the content, each `<section>` starts a new page and gets a line in the table
of contents.

`html2pdf --help` lists everything: the paper (`--size`, `--landscape`),
margins in millimetres (`--start` for the first page's top, 60 by default;
`--top` 10, `--right` 10, `--bottom` 10, `--left` 30), `--header`, `--footer`,
`--css`, `--toc`, `--base-url`, `--title`, `--author`, the passwords,
`--builtin-fonts` and `--draw-margins` (outlines the margins, header and footer
while you get a layout right). Give `-` as the input to read HTML from stdin, or
as the output to write the PDF to stdout.

## Installing

### Ubuntu (22.04, 24.04 and 26.04)

From the simply-cpp apt repository:

```bash
curl -fsSL https://apt.roelof.co.za/setup.sh | bash
sudo apt -y install html2pdf
```

### macOS

From the simply-cpp Homebrew tap:

```bash
curl -fsSL https://apt.roelof.co.za/setup.sh | bash    # adds the tap
brew install html2pdf
```

### From source

`bash scripts/install.sh` builds, tests, packages and installs html2pdf (to
`/usr/local/bin`). It needs
[sc-html2pdf](https://github.com/roelofrossouw/simply-cpp-html2pdf).

## How it works, and thanks

html2pdf is a small program on top of
[sc-html2pdf](https://github.com/roelofrossouw/simply-cpp-html2pdf), part of
[simply-cpp](https://github.com/roelofrossouw/simply-cpp). There's no browser
underneath. The HTML and CSS are laid out by
**[litehtml](https://github.com/litehtml/litehtml)**, a lightweight HTML
rendering engine by **Yuri Kobets ([tordex](https://github.com/tordex))** and
its contributors. Many thanks to them: html2pdf wouldn't exist without it. The
PDF is then written by simply-cpp's own PDF writer.

html2pdf is Apache-2.0; litehtml is BSD-3-Clause.
