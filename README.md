XML Parser
============================

XML を解析し、表示する
----------------------------

インストール方法

    $ make

実行ファイル xp が出来る。
以下のような xml ファイルを解析する。

    $ cat text/test2.xml
    <?xml version="1.0" encoding="UTF-8" ?>
    <!DOCTYPE erlref SYSTEM "erlref.dtd">
    <xml>
        <a id="a_id" class="a_class">hoge</a>
        <c>
          <start>key</start>
          <end>value</end>
        </c>
        <d>
          <hoge>hoge desu</hoge>
          <fuga>
            <teketeke>fuwafuwa</teketeke>
          </fuga>
        </d>
    </xml>

xp を実行すると中の要素を見やすく木構造で出力してくれる。

    $ ./xp text/fault.xml
    Show xml tree ... text/test2.xml
    Version ... 1.0  Encoding ... UTF-8"
    Doctype ...  erlref SYSTEM "erlref.dtd"
    [0]:  |
    [0]:   -- xml
    [1]:      |
    [1]:       -- a
    [1]:          - [class] a_class
    [1]:          - [id] a_id
    [2]:      |   |
    [2]:      |    -- hoge
    [1]:      |
    [1]:       -- c
    [2]:      |   |
    [2]:      |    -- start
    [3]:      |   |   |
    [3]:      |   |    -- key
    [2]:      |   |
    [2]:      |    -- end
    [3]:      |       |
    [3]:      |        -- value
    [1]:      |
    [1]:       -- d
    [2]:          |
    [2]:           -- hoge
    [3]:          |   |
    [3]:          |    -- hoge desu
    [2]:          |
    [2]:           -- fuga
    [3]:              |
    [3]:               -- teketeke
    [4]:                  |
    [4]:                   -- fuwafuwa

