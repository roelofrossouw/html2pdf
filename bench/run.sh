#!/bin/bash
# Runs each document through each tool: best of three wall times, peak memory, pages, file size.
cd "$(dirname "$0")"
M="-T 10 -B 10 -L 10 -R 10"
run() { # label doc out command...
    local label=$1 doc=$2 out=$3; shift 3
    local best= mem= ok=1
    for i in 1 2 3; do
        rm -f "$out"
        /usr/bin/time -f "%e %M" -o t.txt "$@" >/dev/null 2>err.txt || ok=0
        read t m < <(tail -1 t.txt)
        if [ -z "$best" ] || awk "BEGIN{exit !($t < $best)}"; then best=$t; fi
        mem=$m
    done
    local pages=$(pdfinfo "$out" 2>/dev/null | awk '/^Pages:/{print $2}')
    local size=$(stat -c %s "$out" 2>/dev/null)
    local fonts=$(pdffonts "$out" 2>/dev/null | tail -n +3 | awk '{e=($0 ~ / yes +yes +yes/ || $(NF-4)=="yes")?"emb":"builtin"; print $1}' | sed 's/^[A-Z]*+//' | sort -u | tr '\n' ',' )
    printf "%s\t%s\t%ss\t%dMB\t%sp\t%dKB\t%s\t%s\n" "$doc" "$label" "$best" $((mem / 1024)) "${pages:-?}" $(( ${size:-0} / 1024 )) "$fonts" "$([ $ok = 1 ] || head -c 80 err.txt | tr '\n' ' ')"
}
for d in 1-hello 2-long 3-styled 4-images 5-sections 6-headers 7-large; do
    hf=""; x=$d.html; wk=""
    if [ $d = 6-headers ] || [ $d = 7-large ]; then
        hf="-H 6-header.html -f 6-footer.html"; x=$d-x.html; wk="--header-html 6-header-wk.html --footer-html 6-footer-wk.html"
        [ $d = 6-headers ] && x=6-headers-x.html
    fi
    run html2pdf $d o-h.pdf html2pdf $d.html o-h.pdf -s 10 -l 10 $hf
    run html2pdf-builtin $d o-hb.pdf html2pdf $d.html o-hb.pdf -s 10 -l 10 --builtin-fonts $hf
    run wkhtmltopdf $d o-w.pdf wkhtmltopdf -q -s A4 $M --enable-local-file-access $wk $d.html o-w.pdf
    run xhtml2pdf $d o-x.pdf xhtml2pdf3 -q $x o-x.pdf
    cp o-h.pdf out-$d-html2pdf.pdf; cp o-hb.pdf out-$d-builtin.pdf; cp o-w.pdf out-$d-wk.pdf 2>/dev/null; cp o-x.pdf out-$d-x.pdf 2>/dev/null
done
