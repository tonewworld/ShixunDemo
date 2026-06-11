# gamedemo

Developed with Unreal Engine 4
# 第三人称角色 C++ 实现（UE 4.27）

## 指引

### 代码位置
- `Source/gamedemo/MyThirdPersonCharacter.h` + `.cpp`
  - **移动逻辑**：`MoveForward` / `MoveRight` + 鼠标 `Turn` / `LookUp`
  - **血量系统**：`Health` / `MaxHealth` + `ApplyDamage` + `OnHealthChanged` 委托
  - **输入绑定**：`SetupPlayerInputComponent` 中绑定 WASD / 空格 / F / Q / 鼠标左右键
  - **能量UI转发**：`GetTimeEnergyPercentage()` / `IsTimeReversing()` 供蓝图绑定
- `Source/gamedemo/GrabComponent.h` + `.cpp`
  - **抓取系统**：`TryGrab` / `ReleaseGrab` + 推拉控制 `StartPush` / `StartPull`
  - **接口检测**：抓取前检测 `IMagneticInteractable` 接口 + 质量 `< 500kg`
  - **跟随逻辑**：`TickComponent` 中每帧将抓取物体移动到相机前方
  - **距离调整**：根据 `bIsPushing` / `bIsPulling` 改变 `GrabDistance`，范围限制 `MinGrabDistance` ~ `MaxGrabDistance`
- `Source/gamedemo/TimeComponent.h` + `.cpp`
  - **时间回溯**：记录位置/旋转/速度，2倍速回放
  - **能量系统**：满能 300，每秒消耗 100（持续3秒），每秒恢复 30（10秒回满）
  - **记录时长**：`MaxRecordTime = 6.0f`（2倍速回溯3秒需要6秒记录）
  - **修复**：遍历所有 StaticMesh 组件而非仅取 `[0]`
- `Source/gamedemo/MagneticInteractable.h`（C++ 接口）
  - 接口函数：`OnGrabbed` / `OnMagneticRelease` / `ApplyMagneticForce` / `IsGrabbedBy`

### 基础功能
- 第三人称跟随相机：`SpringArmComponent` + `CameraComponent`，鼠标控制视野
- 角色朝向鼠标方向移动（`bUseControllerDesiredRotation = true`）
- 血量变化使用动态多播委托
- 按数字键 **1** 测试扣血

### 能力一：时间回溯（Time Rewind）
- **触发**：按住 **F** 键开始回溯，松开停止
- **效果**：角色沿历史路径向过去移动（仅回溯自身，世界不受影响）
- **能量限制**：满能300，每秒消耗100（最多回溯3秒），松开后每秒恢复30（10秒回满）
- **UI绑定**：使用 `GetTimeEnergyPercentage()`（返回 0~1）绑定 Progress Bar，`IsTimeReversing()` 切换颜色
- **数据记录**：每帧记录位置 + 旋转 + 线速度 + 角速度，储存最多6秒

### 能力二：磁力抓取（Magnetic Grasp）
- **触发**：按一下 **Q** 切换抓取模式，再按一下 Q 释放
- **射线检测**：从屏幕中心发射射线，检测 `IMagneticInteractable` 接口 + 质量 `< 500kg`
- **鼠标控制**：移动视角控制物体相对相机的方向
- **推远**：抓取状态下，按住 **鼠标左键** → 物体逐渐远离相机
- **拉近**：抓取状态下，按住 **鼠标右键** → 物体逐渐靠近相机
- **准心反馈**：未抓取时白色，抓取成功变绿色
- **物理移动**：支持速度驱动或直接设置位置（`bUsePhysics`）

### 输入配置（需手动添加）
在项目设置 → Input → Action Mappings 中添加：
| Action Name | 键位               |
|-------------|-------------------|
| `Grab`      | Q                 |
| `TimeReverse` | F               |
| `Push`      | Left Mouse Button |
| `Pull`      | Right Mouse Button|
| `TestDamage`| 数字键1           |
| `Jump`      | Space Bar         |
| `MoveForward`/`MoveRight` | W/A/S/D |
| `Turn`/`LookUp` | 鼠标移动       |

### 注意事项
- 推拉速度可通过 `GrabComponent` 中的 `PushPullSpeed` 调节（默认 200 单位/秒）
- 最近抓取距离 `MinGrabDistance` 默认 50，最远 `MaxGrabDistance` 默认 800
- 释放时自动停止推/拉状态，避免残留影响
- 可抓取物体需要在蓝图类设置中实现 `MagneticInteractable` 接口（类设置 → 接口 → 添加）