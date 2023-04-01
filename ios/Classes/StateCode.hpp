// 현재 상태 코드를 가져오기 위한 열거형입니다.
enum StateCode : int
{
    // 현재 상태를 정의합니다.
    NotInitialize = 0x00, // 초기화되지 않은 상태입니다.

    NotReadyTarget = 0x10, // 대상 데이터 검색 중인 상태입니다.
    ReadyToProcess = 0x20, // 쿼리를 처리할 준비가 된 상태입니다.

    Processing = 161, // 쿼리 처리 중인 상태입니다.
    Busy = 33,        // 작업 중인 상태입니다.
};