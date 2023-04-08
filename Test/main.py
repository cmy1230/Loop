import time
import os
import uiautomation
import uiautomation as automation
import pyautogui

cgrapath = 'C:\\Users\\asus\\OneDrive\\桌面\\cgra-mg\\CGRA.v'#Chisel生成的Verilog文件位置
vspath = 'D:\\project\\Project4\\Project4\\CGRA.v'#C++程序所处理的Verilog文件位置
resultpath = 'D:\\project\\Project4\\Project4\\result.txt'#记录运行时间文件位置
result1path = 'D:\\project\\Project4\\Project4\\result1.txt'#记录电路规模文件位置
endpath='D:\\project\\Project4\\Project4\\end.txt'#C++程序运行结束标记文件位置

if os.path.exists(cgrapath):
    os.remove(cgrapath)
os.startfile(resultpath)
result = uiautomation.WindowControl(Name="result.txt - 记事本")
result.SetActive()
pyautogui.hotkey('ctrl', 'a')
pyautogui.hotkey('backspace')
pyautogui.hotkey('ctrl', 's')
result.ButtonControl(Name="关闭").Click()
os.startfile(result1path)
result = uiautomation.WindowControl(Name="result1.txt - 记事本")
result.SetActive()
pyautogui.hotkey('ctrl', 'a')
pyautogui.hotkey('backspace')
pyautogui.hotkey('ctrl', 's')
result.ButtonControl(Name="关闭").Click()
#初始化，删除以前的数据记录
idea = uiautomation.WindowControl(Name="CGRA-MG – CGRAMG.scala [CGRA-MG]")
if not idea.Exists(1):
    idea = uiautomation.WindowControl(Name="CGRA-MG – cgra_spec.json [CGRA-MG]")
#定位IDEA窗口，会因为名称不同而改变，这里只使用两个Chisel文件，故只考虑两种情况下的窗口名称
for j in range(2, 10):#电路规模行参数
    idea.SetActive()
    pyautogui.click(2032, 137)
    pyautogui.click(1034, 232)
    pyautogui.hotkey('backspace')
    pyautogui.typewrite('%d' % j)
    #修改Chisel文件中的电路行参数
    for i in range(2, 10):#电路规模列参数
        for k in range(2, j):#防止同一规模电路多次测量影响后续分析
            if i*j <= k*9 and (i*j) % k == 0:
                break
        else:
            if os.path.exists(endpath):
                os.remove(endpath)
            idea.SetActive()
            pyautogui.click(2032, 137)
            pyautogui.click(1069, 277)
            pyautogui.hotkey('backspace')
            pyautogui.typewrite('%d' % i)
            #修改Chisel文件中的电路规模列参数
            pyautogui.click(1755, 134)
            time.sleep(1)#消除界面加载延迟的影响
            pyautogui.click(2205, 82)
            #IDEA窗口内控件无法捕捉，故只能用坐标记录
            while not os.path.exists(cgrapath):
                time.sleep(1)
            #通过检测文件生成来确定Chisel运行是否结束
            os.startfile(cgrapath)
            verilog = uiautomation.WindowControl(Name="CGRA.v - 记事本")
            verilog.ButtonControl(Name="最大化").Click()
            pyautogui.hotkey('ctrl', 'a')
            pyautogui.hotkey('ctrl', 'c')
            verilog.ButtonControl(Name="关闭").Click()
            os.startfile(vspath)
            verilog = uiautomation.WindowControl(Name="CGRA.v - 记事本")
            verilog.ButtonControl(Name="最大化").Click()
            pyautogui.hotkey('ctrl', 'a')
            pyautogui.hotkey('ctrl', 'v')
            pyautogui.hotkey('ctrl', 's')
            verilog.ButtonControl(Name="关闭").Click()
            #Verilog文件内容复制
            os.remove(cgrapath)
            vs = uiautomation.WindowControl(Name="Project4 - Microsoft Visual Studio")
            vs.SetActive()
            pyautogui.click(412, 403)
            pyautogui.hotkey('backspace')
            pyautogui.typewrite('%d' % i)
            pyautogui.click(406, 431)
            pyautogui.hotkey('backspace')
            pyautogui.typewrite('%d' % j)
            pyautogui.hotkey('ctrl', 's')
            vs.SplitButtonControl(Name="调试目标").Click()
            vs = uiautomation.WindowControl(Name="Microsoft Visual Studio 调试控制台")
            while not os.path.exists(endpath):
                time.sleep(1)
            #通过检测标记文件end是否生成确定C++程序是否运行结束

