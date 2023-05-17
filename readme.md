Different versions of this board are shown for each unique IP address connecting. If you post something from home, it is unlikely that anybody will ever see it (except perhaps your imouto in a few years...). If you post something at school, work, or a hotel, however, anyone else on the network connecting from the same IP will also see and be able to reply. In this way, boards are vaguely tied to geographic (or at least net-geographic) locations. The board code you see (e.g. /16777343/) is an integer representation of your real IP -- 127.0.0.1.

The goal is to create a simple system for the creation and upkeep of communities with some kind of locality constraint. A potential next step would be to expose /four/ boards for each connection -- one unique to that fully qualified IPv4 (i.e. exactly A.B.C.D), then one for each higher subclass (A.B.C.*, A.B.*.*, and A.*.*.*).

critical todo:
* ~~CAPTCHA~~
* post cooldown
![example image](https://github.com/kurisufriend/IPchan/blob/master/static/imgs/example.png?raw=true)
