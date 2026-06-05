# gamedemo

Developed with Unreal Engine 4
# 第三人称角色 C++ 实现（UE 4.27）

## 指引

### 代码位置
- `Source/gamedemo/MyThirdPersonCharacter.h` + `.cpp`
  - **移动逻辑**：`MoveForward` / `MoveRight` + 鼠标 `Turn` / `LookUp`
  - **血量系统**：`Health` / `MaxHealth` + `ApplyDamage` + `OnHealthChanged` 委托
  - **输入绑定**：`SetupPlayerInputComponent` 中绑定 WASD / 空格 / 数字键1

### 
- 第三人称跟随相机：`SpringArmComponent` + `CameraComponent`，鼠标控制视野
- 角色朝向鼠标方向移动（`bUseControllerDesiredRotation = true`）
- 血量变化使用动态多播委托
- 按数字键 **1** 测试扣血
