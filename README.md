# gamedemo

Developed with Unreal Engine 4
# 第三人称角色 C++ 实现（UE 4.27）

## 指引

## 指引

### 代码位置
- `Source/gamedemo/MyThirdPersonCharacter.h` + `.cpp`
  - **移动逻辑**：`MoveForward` / `MoveRight` + 鼠标 `Turn` / `LookUp`
  - **血量系统**：`Health` / `MaxHealth` + `ApplyDamage` + `OnHealthChanged` 委托
  - **输入绑定**：`SetupPlayerInputComponent` 中绑定 WASD / 空格 / 数字键1
- `Source/gamedemo/GrabComponent.h` + `.cpp`
  - **抓取系统**：`TryGrab` / `ReleaseGrab` + 推拉控制 `StartPush` / `StartPull`
  - **跟随逻辑**：`TickComponent` 中每帧将抓取物体移动到相机前方
  - **距离调整**：根据 `bIsPushing` / `bIsPulling` 改变 `GrabDistance`，范围限制 `MinGrabDistance` ~ `MaxGrabDistance`
- `Source/gamedemo/TimeComponent.h` + `.cpp`（时间回溯功能，未修改）

### 
- 第三人称跟随相机：`SpringArmComponent` + `CameraComponent`，鼠标控制视野
- 角色朝向鼠标方向移动（`bUseControllerDesiredRotation = true`）
- 血量变化使用动态多播委托
- 按数字键 **1** 测试扣血

### 抓取与推拉系统
- **抓取**：按住 **Q** 键 → 射线检测抓取物理物体，物体跟随相机前方（保持初始距离）
- **推远**：抓取状态下，按住 **鼠标左键** → 物体逐渐远离相机（距离增加）
- **拉近**：抓取状态下，按住 **鼠标右键** → 物体逐渐靠近相机（距离减小）
- **释放**：松开 **Q** 键 → 物体释放，恢复重力
- **准心反馈**：未抓取时白色，抓取成功变绿色
- **物理移动**：支持两种模式（`bUsePhysics`）：速度驱动或直接设置位置

### 输入配置（需手动添加）
在项目设置 → Input → Action Mappings 中添加：
| Action Name | 键位               |
|-------------|-------------------|
| `Grab`      | Q                 |
| `Push`      | Left Mouse Button |
| `Pull`      | Right Mouse Button|
| `TestDamage`| 数字键1           |
| `TimeReverse`| （根据原有设置）   |
| `Jump`      | Space Bar         |
| `MoveForward`/`MoveRight` | W/A/S/D |
| `Turn`/`LookUp` | 鼠标移动       |

### 注意事项
- 推拉速度可通过 `GrabComponent` 中的 `PushPullSpeed` 调节（默认 200 单位/秒）
- 最近抓取距离 `MinGrabDistance` 默认 50，最远 `MaxGrabDistance` 默认 800
- 释放时自动停止推/拉状态，避免残留影响