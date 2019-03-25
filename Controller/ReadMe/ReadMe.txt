2018-12-06
1.添加伺服电机刹车程序[task_brake_servo]
2.485串口波特率由9600改为115200

2018-12-27
1.task_brake_servo加入ABS功能
2.task_upload上传数据加入加速度fDec

2019-01-25
1.task_brake_servo添加使用转矩方式刹车
2.task_upload.h中结构体变量uploadData添加res1、res2使之与最长变量类型float对齐

2019-02-14
1.task_brake_servo刹车使用转矩模式，松刹车使用速度模式