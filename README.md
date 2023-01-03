# proxy-server(작성 중)
광운대학교 21학년도 1학기 시스템프로그래밍 강의를 통해 진행한 프로젝트로, 소켓 프로그래밍을 통해 프록시 서버를 구현한 프로젝트입니다. 

## 프로젝트 설명
프록시 서버란 클라이언트의 요청을 대신 처리하는 서버로, **캐싱**을 이용하여 효율적으로 동작합니다.   
본 프로젝트에서 구현한 프록시 서버는 사용자의 요청을 다음과 같이 처리합니다.  

### MISS
먼저 사용자의 요청이 캐시 데이터로 저장되어 있지 않은 경우(MISS), 웹서버로 request를 전송하고 그 response를 사용자에게 전달 및 캐시 데이터로 저장합니다.   
<p align = "center">
<img src = "https://user-images.githubusercontent.com/71579787/210266488-4e092e85-3b04-4ecb-869d-bdceff19a4b9.png" width = "600">
</p>

### **HIT**
사용자의 요청이 캐시 데이터로 저장되어 있는 경우(HIT), 캐시 데이터로 저장되어 있는 데이터를 사용자의 request에 대한 response로 전달합니다.

<p align = "center">
<img src = "https://user-images.githubusercontent.com/71579787/210267031-7c92f7c2-04d9-491b-a526-5b6a19a21c3d.png" width = "580">
</p>

### 멀티 쓰레드
멀티 쓰레드를 이용하여 다양한 사용자의 요청을 병렬적으로 처리합니다.  
메인 쓰레드에서는 사용자의 요청을 기다리며 새로운 요청마다 새로운 쓰레드를 할당하고, 자식 쓰레드에서는 사용자의 요청에 따라 응답을 반환합니다. 

**메인 쓰레드**
* 사용자의 요청을 받기 위한 소켓 생성
* 사용자 요청 대기
* 사용자의 요청 발생 시 자식 쓰레드 생성
* 자식 쓰레드 생성 후 다시 사용자 요청 대기

**자식 쓰레드**
* 사용자의 요청이 MISS인 경우, 웹 서버에 요청을 전달한 후 응답을 캐싱 및 반환
* 사용자의 요청이 HIT인 경우, 캐시 데이터에 저장된 응답을 반환
* 응답을 반환한 후 종료

## 실행
### 설정(Firefox)
* **브라우저의 HTTP Proxy 설정**<br>
[Preferences]-[Genenral]-[Network Proxy] - [settings]에서 HTTP proxy를 local IP, port 번호 38030으로 설정
<p align = "center">
<img src = "https://user-images.githubusercontent.com/71579787/210311845-cb682ee7-7c47-40c3-bdf0-d378e6aafe09.png" width = "400">
</p>

* **브라우저 캐시 데이터 삭제(매 검색마다)**<br>
[Preferences]-[Privacy & Security]에서 History와 Cookies and Site Data 제거
<p align = "center">
<img src = "https://user-images.githubusercontent.com/71579787/210312302-4ebf4d95-83be-4c5d-a9ae-abdfb6ef2a23.png" width = "400">
</p>

### 실행
1. 브라우저의 사이트 요청(MISS) - ※HTTP 접근이 가능한 사이트(ex) http://me.go.kr/home/web/main.do)
2. 브라우저 캐시 삭제
3. 1과 동일한 사이트 요청(HIT)


## 개발환경
* 언어: C
* 운영체제: Ubuntu 16.04.5 (VMware Workstation 15 Player)
* 브라우저: Firefox

