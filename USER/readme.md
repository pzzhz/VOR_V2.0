<!--
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2025-02-06 15:16:31
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2025-02-06 16:52:42
 * @FilePath: \USER\readme.md
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
-->
25-02-06

优先处理pause 显示不同步问题 
根据需求修改pause功能
    1.改成每次点击pause后重新回到原点
    2.和汪老师协商暂停后如何操作
    3.目前先：
        暂停后不停止录像，再次启动后从原定启动，将vor的暂停重新定位未相位0点

    修改细节
        1.control中的pause从高低电平表达转变为上升沿触发，每个模式的状态机自行处理
        2.implement 返回当前运动的状态（running pausing paused）

    测试内容：
    ok  1.测试pause -》在电机停止前不可触摸
    ok  2.相机在pause后暂停拍摄，并在重新启动后可正常录像，运行进度在重新启动后归零

25-02-09

完成了tc 和vhit模式的修改 

    测试内容：
        1.测试tc 速度曲线和vhit曲线