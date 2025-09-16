#define APP1 1 //Car Control
#define APP2 2 //RGB Control
#define APP3 3 //Servo Control
#define SELECTED_AGENT APP3

extern int car_control_agent();
extern int rgb_control_agent();
extern int servo_control_agent();

using namespace std;

int main()
{
#if SELECTED_AGENT == APP1
	car_control_agent();
#elif SELECTED_AGENT == APP2
	rgb_control_agent();
#elif SELECTED_AGENT == APP3
	servo_control_agent();
#else
	printf("NO AGENT APP SELECTED");
#endif 

	return 0;
} 