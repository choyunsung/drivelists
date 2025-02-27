<!-- Make sure you edit doc/README.hbs rather than README.md because the latter is auto-generated -->
<!-- README.md는 자동 생성되므로 doc/README.hbs를 수정해야 합니다 -->

drivelists
=========

> List all connected drives in your computer, in all major operating systems.
> 모든 주요 운영 체제에서 컴퓨터에 연결된 모든 드라이브를 나열합니다.

Supports (지원 운영체제):

- Windows
- GNU/Linux distributions that include [util-linux](https://github.com/karelzak/util-linux) and [udev](https://wiki.archlinux.org/index.php/udev)
- Mac OS X

When the user executes `drivelists.list()`, the module checks the operating
system of the client and executes the corresponding drive scanning script.
사용자가 `drivelists.list()`를 실행하면 모듈이 클라이언트의 운영 체제를 확인하고 해당하는 드라이브 스캔 스크립트를 실행합니다.

Example (예제):

```js
const drivelists = require('drivelists');

const drives = await drivelists.list();
console.log(drives);
```

Installation (설치)
------------

```sh
$ npm install --save drivelists
```

Documentation (문서)
-------------

### drivelists~list()
**Summary**: List available drives (사용 가능한 드라이브 목록 표시)  
**Example**
```js
const drivelists = require('drivelists');

const drives = await drivelists.list();
drives.forEach((drive) => {
  console.log(drive);
});
```

License (라이선스)
-------

The project is licensed under the Apache 2.0 license.
이 프로젝트는 Apache 2.0 라이선스로 제공됩니다.
