// 현재 상태 코드를 가져오기 위한 열거형입니다.
enum StateCode : int
{
    // 현재 상태를 정의합니다.
    NotInitialized = -1,  // 초기화되지 않은 상태입니다.
    WaitingForTarget = 0, // 대상 데이터 검색 중인 상태입니다.
    ReadyToProcess = 1,   // 쿼리를 처리할 준비가 된 상태입니다.
    Processing = 2,       // 쿼리 처리 중인 상태입니다.
    Busy = 3,             // 작업 중인 상태입니다.
};