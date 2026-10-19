// html2pdf: an HTML file as a PDF, through sc::html2pdf (litehtml and PDFHummus): pages with a
// header and footer, page numbers, a table of contents, links and bookmarks. --help lists the
// options.

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include <core.h>
#include <params.h>

#include <html2pdf.h>

namespace {
    std::string read(const std::string &file) {
        if (!std::filesystem::is_regular_file(file)) throw std::runtime_error{"can't read " + file};
        return sc::file_get_contents(file);
    }
}

int main(int argc, char **argv) {
    const sc::params args{argc, argv, {
        {"name", "html2pdf"},
        {"version", HTML2PDF_VERSION},
        {"description", "Writes an HTML file as a PDF. Without OUTPUT it's written next to INPUT, as .pdf. In the header and "
                        "footer, #Page#, #ToPage#, #section#, #subsection# and #subtitle# are filled in per page; <section> in "
                        "the content starts a page and an entry in the table of contents."},
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
            {"base-url", {{"short", "u"}, {"value", "URL"}, {"help", "what relative links and images are found against (default: INPUT's folder)"}}},
            {"title", {{"value", "TEXT"}, {"help", "the document's title (default: the HTML's <title>)"}}},
            {"author", {{"value", "TEXT"}, {"help", "the document's author"}}},
            {"password", {{"short", "p"}, {"value", "TEXT"}, {"help", "the owner's password (default: the user password)"}}},
            {"user-password", {{"short", "w"}, {"value", "TEXT"}, {"help", "the password to open it; readers may only print it"}}},
            {"draw-margins", {{"type", "boolean"}, {"help", "outline the margins, header and footer, to check a layout"}}}}},
        {"positional", {
            {"input", {{"help", "the HTML file"}}},
            {"output", {{"required", false}, {"help", "the PDF to write"}}}}}}};

    try {
        const std::filesystem::path input = args.as<std::string>("input");
        const std::filesystem::path given = args.as<std::string>("output", "");
        const auto output = given.empty() ? std::filesystem::path{input}.replace_extension(".pdf") : given;

        sc::html2pdf::options settings;
        settings.size = sc::page_size_from(args.as<std::string>("size"));
        settings.turned = args.as<bool>("landscape") ? sc::orientation::landscape : sc::orientation::portrait;
        settings.margins = {args.as<double>("top"), args.as<double>("right"), args.as<double>("bottom"), args.as<double>("left"),
                            args.as<double>("start")};
        if (const auto header = args.as<std::string>("header", ""); !header.empty()) settings.header = read(header);
        if (const auto footer = args.as<std::string>("footer", ""); !footer.empty()) settings.footer = read(footer);
        if (const auto css = args.as<std::string>("css", ""); !css.empty()) settings.css = read(css);
        settings.toc_page = static_cast<unsigned>(std::max(0, args.as<int>("toc")));
        settings.html.base_url = args.as<std::string>("base-url", std::filesystem::absolute(input).string());
        if (const auto title = args.as<std::string>("title", ""); !title.empty()) settings.metadata["Title"] = title;
        if (const auto author = args.as<std::string>("author", ""); !author.empty()) settings.metadata["Author"] = author;
        settings.metadata["Creator"] = "html2pdf " HTML2PDF_VERSION;
        const auto owner = args.as<std::string>("password", ""), user = args.as<std::string>("user-password", "");
        if (!owner.empty() || !user.empty()) settings.protection = sc::pdf::protection{.user_password = user, .owner_password = owner};
        settings.draw_margins = args.as<bool>("draw-margins");

        const sc::html2pdf pdf{read(input.string()), settings};
        pdf.save(output);
        std::cout << output.string() << ": " << pdf.page_count() << (pdf.page_count() == 1 ? " page\n" : " pages\n");
    } catch (const std::exception &error) {
        std::cerr << "html2pdf: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
