Pebble Hangul Watch
===================

![First screenshot](https://raw2.github.com/andrwj/pebble-hangul-watch/master/screenshot.jpg)
--
페블용 한글시계는 [@krazyeom](http://twitter.com/krazyeom)님의 아이디어로 시작되었고, [@drunkhacker](http://twitter.com/drunkhacker)님이 구현하였으며, [@andrwj](http://twitter.com/andrwj)가 SDK2에 맞춰 변환하였고 몇가지 옵션을 제공하도록 수정/추가 하였습니다. 

v1.3.0
* [내려받기 v1.3.0](https://github.com/andrwj/pebble-hangul-watch/blob/master/pebble-hangul-watch-v1.3.0.pbw) 
* 버튼만으로 조작이 가능한 버전입니다. (1.2.x 버전은 PebbleKit Javascript API를 사용하고 있기 때문에 페블앱스토어를 통해서만 설치할 수 있습니다)
* 원 제작자이신 [@drunkhacker](http://twitter.com/drunkhacker)님이 앱스토어에 직접 올릴 수 있으므로 저는 이 버전으로 마무리합니다.
* Watchface의 경우에는 위/아래/선택 버튼에 대해 동작을 지정할 수 없으므로 BACK 버튼과 흔들기로 설정을 변경합니다.
* BACK 버튼 짧게 누르기: 도트 감소 1 ~ 8
* BACK 버튼 길게 누르기: 배경색 변경
* 흔들기: 배경색 변경

v1.2.1
* [내려받기 v1.2.1](https://raw2.github.com/andrwj/pebble-hangul-watch/master/pebble-hangul-watch-v1.2.1.pbw) 
* 배경색 변경을 지원합니다.
* 도트 생략정도를 선택할 수 있습니다.
* 흔들거나 탭을 하여 배경색상및 글자색을 뒤집을 수 있습니다 (2초 이내에 `tap` 두번)

![normal1](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/1.png)   ![normal2](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/2.png)   ![normal3](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/3.png)   ![normal4](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/4.png)   ![normal5](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/5.png)   ![normal6](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/6.png)   ![normal7](https://raw.github.com/andrwj/pebble-hangul-watch/master/docs/7.png)   

   
![reverse1](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r1.png)   ![reverse2](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r2.png)   ![reverse3](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r3.png)   ![reverse4](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r4.png)   ![reverse5](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r5.png)   ![reverse6](https://raw2.github.com/andrwj/pebble-hangul-watch/master/docs/r6.png)   ![reverse7](https://raw.github.com/andrwj/pebble-hangul-watch/master/docs/r7.png)   

![Preference Panel](https://raw.github.com/andrwj/pebble-hangul-watch/master/docs/hangul-watch-preferences.png)


##### Converted to SDK 2.0 
* by A.J <andrwj@gmail.com>
* fixed bug - application crash when midnight 
* reduce dots of inactive characters for readability
* [Download v1.1.0](https://raw2.github.com/andrwj/pebble-hangul-watch/master/pebble-hangul-watch.pbw)   
