
/*----------------------------------------------------*/
#define IN1 2 // CHÂN IN1 KÊNH A , D2
#define IN2 3 // CHÂN IN2 KÊNH A , D3
#define IN3 4 // CHÂN IN3 KÊNH B , D4
#define IN4 7 // CHÂN IN4 KÊNH B , D7
#define ENA 5 // CHÂN ĐIỀU KHIỂN BĂM XUNG KÊNH A , D5
#define ENB 6 // CHÂN ĐIỀU KHIỂN BĂM XUNG KÊNH B , D6
/*----------------------------------------------------*/
void khaibao(){
    DDRD |= (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4)| (1 << ENA)| (1 << ENB);
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D) | (1 << ADC4D);
     // CÀI ĐẶT FAST PWM CHUẨN CHO TIMER0 (Điều khiển cả 2 kênh ENA và ENB)
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Bật PWM không đảo cho cả OC0A (chân PD6 - ENB) và OC0B (chân PD5 - ENA)
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); 
    TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64
}

int lim(int a , int MAX , int MIN){
    if ( a > MAX) a = MAX;
    if ( a < MIN) a = MIN;
    return a;
}

void motor(int L,int R){
    R = lim(R,255,-255);
    L = lim(L,255,-255);
    // 1. CHIỀU QUAY ĐỘNG CƠ PHẢI (Kênh A: IN1, IN2 - Chân ENA)
    if (L > 0) {
        PORTD |=  (1 << IN1);      // Set IN2 = 1
        PORTD &= ~(1 << IN2);     // Clear IN1 = 0
    } 
    else if (L < 0) {
        PORTD &= ~(1 << IN1);     // Clear IN2 = 0
        PORTD |= (1 << IN2);      // Set IN1 = 1
    } 
    else { // Khi R = 0 (Cho động cơ dừng hẳn)
        PORTD &= ~(1 << IN1);
        PORTD &= ~(1 << IN2);
    }

    // 2. CHIỀU QUAY ĐỘNG CƠ TRÁI (Kênh B: IN3, IN4 - Chân ENB)
    if (R > 0) {
        PORTD |=  (1 << IN4);      // Set IN4 = 1
        PORTD &= ~(1 << IN3);     // Clear IN3 = 0
    } 
    else if (R < 0) {
        PORTD &= ~(1 << IN4);     // Clear IN4 = 0
        PORTD |=  (1 << IN3);      // Set IN3 = 1
    } 
    else { // Khi L = 0 (Cho động cơ dừng hẳn)
        PORTD &= ~(1 << IN3);
        PORTD &= ~(1 << IN4);
    }
    // 3. XUẤT XUNG PWM ĐIỀU KHIỂN TỐC ĐỘ
    // OCR0B tương ứng với chân PD5 (ENA - điều khiển kênh A / Động cơ L)
    OCR0B = abs(L); 
    // OCR0A tương ứng với chân PD6 (ENB - điều khiển kênh B / Động cơ R)
    OCR0A = abs(R);
}

void turn_rigth(){
    motor(255,-255);
}
void turn_left(){
    motor(-255,255);
}
void stop(){
    motor(0,0);
}
void forward(){
    motor(255,255);
}
void backward(){
    motor(-255,-255);
}

void setup() {
    khaibao();
    Serial.begin(9600);
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'F':
                forward();
                break;
            case 'B':
                backward();
                break;
            case 'L':
                turn_left();
                break;
            case 'R':
                turn_rigth();
                break;
            case 'S':
                stop();
                break;
        }
    }
}