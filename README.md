# rapidxmltojson

`rapidxmltojson` converts xml str to json str

```sh
pip install rapidxmltojson
```
## Examples

```py
>>> import rapidxmltojson
>>> data = rapidxmltojson.parse("""
...     <root a="testa">
...         <child>
...             <item>text 1</item>
...             <item>text 2</item>
...         </child>
...         <example b="testb">
...             example text
...         </example>
...         <test-ns:example xmlns:test-ns="http://test-ns.com/">
...             example text (namespaced)
...         </test-ns:example>
...     </root>
... """)

>>> import json
>>> print(json.dumps(json.loads(data), indent=4))
{
    "root": {
        "@a": "testa",
        "child": {
            "item": [
                "text1",
                "text2"
            ]
        },
        "example": {
            "#text": "exampletext",
            "@b": "testb"
        },
        "test-ns:example": {
            "#text": "exampletext(namespaced)",
            "@xmlns:test-ns": "http://test-ns.com/"
        }
    }
}
```
