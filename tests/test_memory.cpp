// Deterministic regression test: every new/new[] made during conversion must
// be paired with delete/delete[], including when parsing fails.
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>

namespace {
std::size_t live_allocations = 0;

void* allocate(std::size_t size)
{
    void* pointer = std::malloc(size ? size : 1);
    if (!pointer)
        throw std::bad_alloc();
    ++live_allocations;
    return pointer;
}

void release(void* pointer) noexcept
{
    if (pointer) {
        --live_allocations;
        std::free(pointer);
    }
}
}

void* operator new(std::size_t size) { return allocate(size); }
void* operator new[](std::size_t size) { return allocate(size); }
void operator delete(void* pointer) noexcept { release(pointer); }
void operator delete[](void* pointer) noexcept { release(pointer); }
#if __cplusplus >= 201402L
void operator delete(void* pointer, std::size_t) noexcept { release(pointer); }
void operator delete[](void* pointer, std::size_t) noexcept { release(pointer); }
#endif

#include "rapidxmltojson.hpp"
#include <vector>

bool convert(const std::string& xml, const std::string& expected)
{
    // RapidXML modifies its input, so each call needs a fresh writable buffer.
    std::vector<char> input(xml.begin(), xml.end());
    input.push_back('\0');
    const std::string actual = xmltojson(input.data());
    if (actual != expected) {
        std::fprintf(stderr, "Expected: %s\nActual: %s\n",
                     expected.c_str(), actual.c_str());
        return false;
    }
    return true;
}

bool check(const char* name, const std::string& xml, const std::string& expected)
{
    if (!convert(xml, expected))
        return false;
    const std::size_t before = live_allocations;
    for (int i = 0; i < 1000; ++i) {
        if (!convert(xml, expected))
            return false;
    }
    if (live_allocations != before) {
        std::fprintf(stderr, "%s: leaked %zu allocations in 1000 conversions\n",
                     name, live_allocations - before);
        return false;
    }
    std::printf("PASS: %s (1000 conversions, no outstanding allocations)\n", name);
    return true;
}

int main()
{
    bool passed = true;
    passed &= check("plain XML", "<root><item>one</item><item>two</item></root>",
                    "{\"root\":{\"item\":[\"one\",\"two\"]}}");
    passed &= check("prefixed root and children",
                    "<n:root xmlns:n=\"urn:test\"><n:item>one</n:item><n:item>two</n:item></n:root>",
                    "{\"n:root\":{\"@xmlns:n\":\"urn:test\",\"n:item\":[\"one\",\"two\"]}}");
    passed &= check("default namespace", "<root xmlns=\"urn:test\"><item>one</item></root>",
                    "{\"root\":{\"@xmlns\":\"urn:test\",\"item\":\"one\"}}");
    passed &= check("unclosed elements", "<root><child>",
                    "Exception: Parse error, Details: unexpected end of data");
    passed &= check("unterminated attribute", "<root a=\"",
                    "Exception: Parse error, Details: expected ' or \"");

    std::string large_namespaced_xml = "<n:root xmlns:n=\"urn:test\">";
    std::string large_expected = "{\"n:root\":{\"@xmlns:n\":\"urn:test\",\"n:item\":[";
    for (int i = 0; i < 1000; ++i) {
        large_namespaced_xml += "<n:item>value</n:item>";
        if (i)
            large_expected += ",";
        large_expected += "\"value\"";
    }
    large_namespaced_xml += "</n:root>";
    large_expected += "]}}";
    passed &= check("large namespaced document", large_namespaced_xml, large_expected);

    // Force the XML pool to grow beyond its inline storage before a parse
    // exception, verifying cleanup of dynamically allocated pool blocks too.
    std::string large_xml = "<root>";
    for (int i = 0; i < 1000; ++i)
        large_xml += "<item/>";
    passed &= check("large malformed document", large_xml,
                    "Exception: Parse error, Details: unexpected end of data");
    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
