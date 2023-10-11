from libcpp.string cimport string


cdef extern from "rapidxmltojson.hpp":
    string xmltojson(char*)


def parse(xml):
    json = xmltojson(xml.encode('utf-8'))
    return json.decode('utf-8')
