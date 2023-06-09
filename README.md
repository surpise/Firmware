# OpenCR로 4거리 신호등 구현하기
## 데모 영상
[YouTube 링크](https://youtu.be/x9XPnMthFAg)

</br>

## 사용한 언어 및 하드웨어
#### 언어
- C
#### 하드웨어
- OpenCR

</br>

## 기본 기능
빨간불, 노란불, 좌회전, 파란불의 LED를 적절하게 제어

4거리 신호등 중 붙어있는 3방향의 신호 각 4개를 제어

각 일정한 시간이 흐른다면 다음 Step으로 넘어감 (파란불, 빨간불, 좌회전은 7초 / 노란불은 2초)

한 세트 = 파란불(7초) -> 노란불(2초) -> 빨간불/좌회전(7초) -> 빨간불/노란불(2초) -> 빨간불(7초)

위의 세트를 무한반복 함

한 세트가 지날때 마다, 메인보드의 LED 4개에 바이너리 카운트를 표시함

1111이 최대이며(15번) 그 다음은 다시 0000으로 표시

메인 보드 내의 스위치 1개는 리셋으로 동작. 중간에 아무때나 이 스위치를 누르면 카운트용 내부 LED는 0000으로 리셋되게 함

스위치는 눌렀다가 뗄떼에만 리셋으로 동작

## 추가 기능
파란불이 꺼질때까지 남은 시간(초단위)를 7 segment로 표시 (1방향 만)

각 거리에 신호에 맞는 횡단보도 LED(빨간불, 파란불)를 설치하고, 파란불이 꺼지기 2초전 빠르게 깜빡이게 함

외부보드에 Skip 스위치 기능 구현. 해당 스위치를 누르면 남아있는 시간에 상관없이 바로 다음 step으로 넘어가게 함

