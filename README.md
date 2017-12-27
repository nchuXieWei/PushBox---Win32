# PushBox---Win32
利用Win32API制作的推箱子游戏，带有自定义关卡功能

PushBoxEditor为关卡编辑器，PushBox为游戏

游戏区域为20 * 20个“格子”
若PushBoxEditor是第一次运行，会自动创建5个关卡，存于PushBoxData.xw中，该文件也是PushBox游戏读取关卡数据的地方

在PushBoxEditor中，通过单击鼠标左键可以顺序地修改“格子”的状态，状态按顺序共有7种：背景、墙、有箱子的格子、有目标的格子、有人的格子、箱子与目标重合的格子、空的格子。为了方便快速编辑关卡，通过单击鼠标右键，可以直接将“格子”修改为空的格子。

在PushBoxEditor中编辑关卡地图后，通过Insert键保存。为了方便快速编辑关卡，可以不人为设置墙的位置，程序将自动补上。
最多支持设置20个关卡，通过UP、DOWN键选择欲编辑的关卡。

在PushBoxEditor中按SHIFT键可以清空当前编辑关卡的地图。


PushBox中通过UP、DOWN、LEFT、RIGHT键控制人物移动，若需重置本关卡，按SHIFT键。
