# Converts tests/page.html with a header, footer and table of contents, and checks the PDF:
# run by ctest with -DHTML2PDF=<the program> -DSOURCE=<tests> -DWORK=<a folder of its own>.
file(REMOVE_RECURSE "${WORK}")
file(MAKE_DIRECTORY "${WORK}")
execute_process(COMMAND "${HTML2PDF}" "${SOURCE}/page.html" "${WORK}/page.pdf" --header "${SOURCE}/header.html"
        --footer "${SOURCE}/footer.html" --toc 2 --author tester
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
if (NOT result EQUAL 0)
    message(FATAL_ERROR "html2pdf failed (${result}): ${errors}")
endif ()
if (NOT output MATCHES "page.pdf: 4 pages")
    message(FATAL_ERROR "expected 4 pages (a title page, the contents and two parts), got: ${output}")
endif ()
# Read as hex: file(READ) as text stops at the first NUL byte, which a font or image stream has.
file(READ "${WORK}/page.pdf" pdf HEX)
foreach (expected "%PDF-" "/Type /Outlines" "/Title (The second part)" "/Author (tester)" "/URI (https://example.com)" "/Dest")
    string(HEX "${expected}" wanted)
    string(FIND "${pdf}" "${wanted}" at)
    if (at EQUAL -1)
        message(FATAL_ERROR "the PDF has no ${expected}")
    endif ()
endforeach ()

# Without OUTPUT, the PDF goes next to the input.
configure_file("${SOURCE}/page.html" "${WORK}/copy.html" COPYONLY)
execute_process(COMMAND "${HTML2PDF}" "${WORK}/copy.html" RESULT_VARIABLE result)
if (NOT result EQUAL 0 OR NOT EXISTS "${WORK}/copy.pdf")
    message(FATAL_ERROR "html2pdf didn't write copy.pdf next to copy.html")
endif ()
