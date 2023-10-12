from libcpp.string cimport string


cdef extern from "rapidxmltojson.hpp":
    string xmltojson(char*)


class RapidXMLError(Exception): ...


def parse(xml):
    json = xmltojson(xml.encode('utf-8'))
    result = json.decode('utf-8')

    if result.startswith('Exception'):
        message = result.replace('Exception: ', '')
        raise RapidXMLError(message)

    return result
