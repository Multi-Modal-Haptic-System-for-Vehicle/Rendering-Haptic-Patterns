infSplitNum
 - loop 안 계산 테스트 위함
 - loop '안'에서 distance 계산, 원 모양 렌더링

ModifiedTactileBrush
 - 면 자극 테스트하기 위함
 - moving tactile stroke X
 - 2개의 행 포함하는 면 이동 렌더링
 - iterateTactile은 6개 진동자 iterNum만큼 반복

SplitStimuli_RTP
 - splitNum이 몇일때 잘 감지할지 테스트하기 위함
 - moving tactile stroke O
 - splitNum 입력, splitNum만큼 쪼개서 2개 행 면이동 렌더링

TestSplit
 - loop 밖에서 distance 계산, 원 모양 렌더링
 - ?

VibrotactileAddFunc
 - 임의의 함수 입력 -> 다양한 궤적 렌더링 테스트하기 위함
 - moving tactile stroke O
 - 함수 y 입력
 - x값 기준 y에 해당하는 함수 모양 패턴 실행

VibrotactileAddPattern
 - 원을 기본으로 해서, 평행이동, 회전 이용하여 여러 패턴 구현 가능한지 테스트하기 위함
 - moving tactile stroke O
 - loop 밖에서 distance 계산
 - ppt에 나와있는 패턴 실행, 1~j까지 구현

VibrotactileAddPattern2
 - 위 코드 간소화 위함
 - moving tactile stroke O
 - loop 밖에서 distance 계산
 - 위 코드에서 movemode추가, 코드 간소화
 - 1~a까지 구현

VibrotactileCircleAngle
 - 원형 궤적 '각도'를 잘 인지할 수 있는지 테스트하기 위함
 - moving tactile stroke O
 - loop 밖에서 distance 계산
 - 0~360까지 입력
 - 0도부터 해당 각도까지 원 모양 렌더링

VibrotactileDiagonal
 - 직선 궤적 vs 대각선 궤적 차이를 잘 인지할 수 있는지 테스트하기 위함
 - moving tactile stroke X
 - 대각선 모양 렌더링

VibrotactileDiagonal2
 - 위 코드 moving tactile stroke 알고리즘 사용하기 위함
 - moving tactile stroke O (입력 : L, R)
 - y=x 함수 렌더링
 - moving tactile stroke인것과 아닌것 비교

VibrotactileDrawCircle
 - 2x3에서 직선 궤적 vs 원형 궤적 차이를 잘 인지할 수 있는지 테스트하기 위함
 - moving tactile stroke O (입력 : O)

VibrotactileDrawCircle
 - 3x3에서 직선 궤적 vs 원형 궤적 차이를 잘 인지할 수 있는지 테스트하기 위함
 - moving tactile stroke O (입력 : O)

VibrotactileDrawFunc
 - addFunc이랑 같음

VibrotactileOpenClose
 - 열리거나 닫히기, 좁아지거나 넓어지기 특이한 패턴 인지 가능한지 테스트하기 위함
 - moving tactile stroke X

VibrotactileOpenClose2
 - 위 코드 moving tactile stroke 알고리즘 사용하기 위함
 - moving tactile stroke O (입력 : N, W)



