// html2pdf: an HTML file as a PDF, through sc::html2pdf (litehtml and sc-pdflite): pages with a
// header and footer, page numbers, a table of contents, links and bookmarks. "-" reads the HTML
// from stdin or writes the PDF to stdout, so it fits in a pipe. --help lists the options.

#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <unistd.h>

#include <core.h>
#include <params.h>

#include <html2pdf.h>

namespace {
    std::string read(const std::string &file) {
        if (file == "-") return {std::istreambuf_iterator<char>{std::cin}, std::istreambuf_iterator<char>{}};
        if (!std::filesystem::is_regular_file(file)) throw std::runtime_error{"can't read " + file};
        return sc::file_get_contents(file);
    }
}

int main(int argc, char **argv) {
    const sc::params args{argc, argv, {
        {"name", "html2pdf"},
        {"version", HTML2PDF_VERSION},
        {"description", "Turns an HTML file into a PDF, its pages broken between lines rather than through them. Give it just "
                        "INPUT and the PDF lands next to it, as a .pdf. INPUT - reads the HTML from stdin and OUTPUT - writes "
                        "the PDF to stdout; with HTML piped in, a lone OUTPUT.pdf is enough: curl -fsSL example.com | html2pdf "
                        "page.pdf. Headers and footers can show #Page#, #ToPage#, "
                        "#section#, #subsection# and #subtitle#, filled in for each page, and every <section> in the content "
                        "starts a new page and an entry in the table of contents. Add --builtin-fonts for PDF's own Helvetica, "
                        "Times and Courier: nothing embedded, smaller files.\nMore on the wiki: "
                        "https://github.com/roelofrossouw/simply-cpp/wiki/HTML-to-PDF"},
        {"options", {
            {"header", {{"short", "H"}, {"value", "FILE"}, {"help", "HTML above the content of every page"}}},
            {"footer", {{"short", "f"}, {"value", "FILE"}, {"help", "HTML below the content of every page"}}},
            {"css", {{"short", "c"}, {"value", "FILE"}, {"help", "a stylesheet for the content, header and footer"}}},
            {"size", {{"default", "A4"}, {"choices", {"A3", "A4", "A5", "letter", "legal"}}, {"help", "the paper"}}},
            {"landscape", {{"type", "boolean"}, {"help", "turn the pages sideways"}}},
            {"start", {{"short", "s"}, {"type", "number"}, {"default", 60}, {"value", "MM"}, {"help", "the first page's top margin"}}},
            {"top", {{"short", "t"}, {"type", "number"}, {"default", 10}, {"value", "MM"}, {"help", "the top margin"}}},
            {"right", {{"short", "r"}, {"type", "number"}, {"default", 10}, {"value", "MM"}, {"help", "the right margin"}}},
            {"bottom", {{"short", "b"}, {"type", "number"}, {"default", 10}, {"value", "MM"}, {"help", "the bottom margin"}}},
            {"left", {{"short", "l"}, {"type", "number"}, {"default", 30}, {"value", "MM"}, {"help", "the left margin"}}},
            {"toc", {{"type", "integer"}, {"default", 0}, {"value", "PAGE"}, {"help", "put a table of contents on this page (0: none)"}}},
            {"base-url", {{"short", "u"}, {"value", "URL"}, {"help", "what relative links and images are found against (default: INPUT's folder, or the current folder for stdin)"}}},
            {"title", {{"value", "TEXT"}, {"help", "the document's title (default: the HTML's <title>)"}}},
            {"author", {{"value", "TEXT"}, {"help", "the document's author"}}},
            {"password", {{"short", "p"}, {"value", "TEXT"}, {"help", "the owner's password (default: the user password)"}}},
            {"user-password", {{"short", "w"}, {"value", "TEXT"}, {"help", "the password to open it; readers may only print it"}}},
            {"builtin-fonts", {{"type", "boolean"}, {"help", "use PDF's built-in fonts (Helvetica, Times, Courier) for text, embedding none: smaller files"}}},
            {"draw-margins", {{"type", "boolean"}, {"help", "outline the margins, header and footer, to check a layout"}}}}},
        {"positional", {
            {"input", {{"help", "the HTML file, or - for stdin"}}},
            {"output", {{"required", false}, {"help", "the PDF to write, or - for stdout (default: INPUT as .pdf)"}}}}}}};

    try {
        std::string input = args.as<std::string>("input");
        std::string output = args.as<std::string>("output", "");
        // curl ... | html2pdf page.pdf: with HTML piped in, a lone .pdf is where it goes.
        if (output.empty() && !isatty(STDIN_FILENO) && sc::lowercase(std::filesystem::path{input}.extension().string()) == ".pdf") {
            output = input;
            input = "-";
        }
        if (output.empty()) {
            if (input == "-") throw std::runtime_error{"give the PDF to write (or - for stdout) when the HTML comes from stdin"};
            output = std::filesystem::path{input}.replace_extension(".pdf").string();
        }
        const auto base = input == "-" ? std::filesystem::current_path() / "" : std::filesystem::absolute(input);

        sc::html2pdf::options settings;
        settings.size = sc::page_size_from(args.as<std::string>("size"));
        settings.turned = args.as<bool>("landscape") ? sc::orientation::landscape : sc::orientation::portrait;
        settings.margins = {args.as<double>("top"), args.as<double>("right"), args.as<double>("bottom"), args.as<double>("left"),
                            args.as<double>("start")};
        if (const auto header = args.as<std::string>("header", ""); !header.empty()) settings.header = read(header);
        if (const auto footer = args.as<std::string>("footer", ""); !footer.empty()) settings.footer = read(footer);
        if (const auto css = args.as<std::string>("css", ""); !css.empty()) settings.css = read(css);
        settings.toc_page = static_cast<unsigned>(std::max(0, args.as<int>("toc")));
        settings.html.base_url = args.as<std::string>("base-url", base.string());
        if (const auto title = args.as<std::string>("title", ""); !title.empty()) settings.metadata["Title"] = title;
        if (const auto author = args.as<std::string>("author", ""); !author.empty()) settings.metadata["Author"] = author;
        settings.metadata["Creator"] = "html2pdf " HTML2PDF_VERSION;
        const auto owner = args.as<std::string>("password", ""), user = args.as<std::string>("user-password", "");
        if (!owner.empty() || !user.empty()) settings.protection = sc::pdf::protection{.user_password = user, .owner_password = owner};
        settings.draw_margins = args.as<bool>("draw-margins");
        settings.builtin_fonts = args.as<bool>("builtin-fonts");

        const sc::html2pdf pdf{read(input), settings};
        const auto pages = std::to_string(pdf.page_count()) + (pdf.page_count() == 1 ? " page\n" : " pages\n");
        if (output == "-") {
            std::cout << pdf.bytes() << std::flush;
            std::cerr << "html2pdf: " << pages; // stdout is the PDF
        } else {
            pdf.save(output);
            std::cout << output << ": " << pages;
        }
    } catch (const std::exception &error) {
        std::cerr << "html2pdf: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
