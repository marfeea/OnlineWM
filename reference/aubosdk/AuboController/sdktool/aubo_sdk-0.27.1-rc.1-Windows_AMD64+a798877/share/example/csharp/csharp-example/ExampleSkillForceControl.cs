using csharp_example;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using static csharp_skill_example.cSharpSkillBinding_TypeDef;

namespace csharp_skill_example
{
    internal class ExampleSkillForceControl
    {
        const int RSERR_SUCC = 0;

        static UInt16 rshd = 0xffff;
        //机械臂IP地址
        const string robot_ip = "192.168.1.20";
        //机械臂端口号
        const int server_port = 30004;

        const int server_port_1 = 30010;


        // 等待机械臂进入目标模式，避免无限阻塞
        static bool WaitForRobotMode(IntPtr robot_state, cSharpBinging_TypeDef.RobotModeType target_mode, int timeoutMs = 30000)
        {
            // 接口调用: 获取当前机械臂的模式
            cSharpBinging_TypeDef.RobotModeType current_mode = cSharpBinging_RobotState.getRobotModeType(robot_state);
            int elapsedMs = 0;
            const int pollIntervalMs = 1000;

            while (current_mode != target_mode)
            {
                if (elapsedMs >= timeoutMs)
                {
                    Console.Error.WriteLine($"等待机械臂进入模式 {target_mode} 超时，当前模式: {current_mode}");
                    return false;
                }
                Console.WriteLine($"机械臂当前模式: {current_mode}");
                Thread.Sleep(pollIntervalMs);
                elapsedMs += pollIntervalMs;
                current_mode = cSharpBinging_RobotState.getRobotModeType(robot_state);
            }
            return true;
        }

        static int exampleStartup(IntPtr cli)
        {
            IntPtr[] robot_names = new IntPtr[10];
            for (int i = 0; i < 10; i++)
            {
                robot_names[i] = Marshal.AllocHGlobal(100); // 分配100字节内存用于存放字符串（考虑 '\0' 结尾）
            }
            try
            {
                int num = cSharpBinding_RPC.rpc_getRobotNames(cli, robot_names);
                if (num <= 0)
                {
                    return -1;
                }

                string robot_name = Marshal.PtrToStringAnsi(robot_names[0]);
                if (string.IsNullOrEmpty(robot_name))
                {
                    return -1;
                }

                IntPtr robot_interface = cSharpBinding_RPC.rpc_getRobotInterface(cli, robot_name);

                // 接口调用: 设置负载
                double mass = 0.0; // 示例质量值
                double[] cog = { 0.0, 0.0, 0.0 }; // 示例重心坐标
                double[] aom = { 0.0, 0.0, 0.0 }; // 示例附加质量矩
                double[] inertia = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // 示例惯性张量

                IntPtr robot_config = cSharpBinging_RobotInterface.robot_getRobotConfig(robot_interface);

                cSharpBinging_RobotConfig.setPayload(robot_config, mass, cog, aom, inertia);

                IntPtr robot_state = cSharpBinging_RobotInterface.robot_getRobotState(robot_interface);
                // 接口调用: 获取机械臂当前模式
                cSharpBinging_TypeDef.RobotModeType robot_mode = cSharpBinging_RobotState.getRobotModeType(robot_state);

                if (robot_mode == cSharpBinging_TypeDef.RobotModeType.Running)
                {
                    Console.WriteLine("机械臂已松刹车，处于运行模式");
                    return 0;
                }

                IntPtr robot_manage = cSharpBinging_RobotInterface.robot_getRobotManage(robot_interface);
                // 接口调用: 机械臂发起上电请求
                cSharpBinging_RobotManage.poweron(robot_manage);


                // 等待机械臂进入空闲模式
                if (!WaitForRobotMode(robot_state, cSharpBinging_TypeDef.RobotModeType.Idle))
                {
                    return -1;
                }

                robot_mode = cSharpBinging_RobotState.getRobotModeType(robot_state);
                Console.WriteLine($"机械臂上电成功，当前模式:{robot_mode}");

                // 接口调用: 机械臂发起松刹车请求
                cSharpBinging_RobotManage.startup(robot_manage);

                // 等待机械臂进入运行模式
                if (!WaitForRobotMode(robot_state, cSharpBinging_TypeDef.RobotModeType.Running))
                {
                    return -1;
                }

                robot_mode = cSharpBinging_RobotState.getRobotModeType(robot_state);
                Console.WriteLine($"机械臂松刹车成功，当前模式: {robot_mode}");

                return 0;
            }
            finally
            {
                for (int i = 0; i < 10; i++)
                {
                    if (robot_names[i] != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(robot_names[i]); // 释放分配的内存
                        robot_names[i] = IntPtr.Zero;
                    }
                }
            }
        }

        static public void exampleInsert(IntPtr rpc_client, IntPtr rtde_client) 
        {
            IntPtr fc = cSharpSkillBinding_ForceControl.create_fc_ptr(rpc_client, rtde_client);
            if (fc == IntPtr.Zero)
            {
                Console.Error.WriteLine("create_fc_ptr failed!");
                return;
            }
            // 设置插孔参数
            cSharpSkillBinding_TypeDef.InsertParams param = new cSharpSkillBinding_TypeDef.InsertParams();
            param.insert_select = cSharpSkillBinding_TypeDef.InsertSelect.z;
            param.hole_diameter = 0.0;
            param.insert_max_speed = 0.005;
            param.insert_time = 1000;
            param.insert_max_force = new cSharpSkillBinding_TypeDef.Vector6d(0.0, 0.0, 10.0, 0.0 ,0.0, 0.0);
            param.insert_max_depth = 0.005;
            param.insert_guide_force = 5;

            param.insert_damp_scale = new cSharpSkillBinding_TypeDef.Vector6d(0.0, 0.0, 0.5, 0.0, 0.0, 0.0);
            param.insert_stiff_scale = new cSharpSkillBinding_TypeDef.Vector6d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

            // 无参考轨迹
            param.guide_traj_type =  cSharpSkillBinding_TypeDef.GuideTrajType.NONE;

            // 螺旋线
            //param.guide_traj_type = cSharpSkillBinding_TypeDef.GuideTrajType.SPIRAL;
            //param.spiral_param.step = 3;
            //param.spiral_param.direction = 1;
            //param.spiral_param.plane = cSharpSkillBinding_TypeDef.SpiralPlane.xy;
            //param.spiral_param.spiral = 0.005;
            //param.spiral_param.helix = 0.005;
            //param.spiral_param.radius = 0.06;

            // 摆线
            //param.guide_traj_type = cSharpSkillBinding_TypeDef.GuideTrajType.WEAVE;
            //param.weave_param.step = 30;
            //param.weave_param.direction = cSharpSkillBinding_TypeDef.WeaveSelect.rx;
            //param.weave_param.amplitude = 0.005;
            //param.weave_param.hold_distance = 0.2;
            //param.weave_param.angle = 0.1;

            cSharpSkillBinding_ForceControl.fcInsert(fc, param);

            // 等待插孔流程结束
            int ret = cSharpSkillBinding_ForceControl.fcWaitCondition(fc);

            if (ret == (int)cSharpSkillBinding_TypeDef.StateCode.Insert_Succeed)
            {
                Console.WriteLine("插孔成功!");
            }
            else if (ret ==  (int)cSharpSkillBinding_TypeDef.StateCode.Insert_MaxForce)
            {
                Console.Error.WriteLine("插孔力过大!");
            }
            else if (ret == (int)cSharpSkillBinding_TypeDef.StateCode.TimeOut)
            {
                Console.Error.WriteLine("插孔超时!");
            }
            else
            {
                Console.Error.WriteLine("Error state code!");
            }

            // 退出力控
            cSharpSkillBinding_ForceControl.fcExit(fc);
            cSharpSkillBinding_ForceControl.destroy_fc_ptr(fc);
        }


        static public void exampleSearch(IntPtr rpc_client, IntPtr rtde_client)
        {
            IntPtr fc = cSharpSkillBinding_ForceControl.create_fc_ptr(rpc_client, rtde_client);
            if (fc == IntPtr.Zero)
            {
                Console.Error.WriteLine("create_fc_ptr failed!");
                return;
            }
            // 设置插孔参数
            cSharpSkillBinding_TypeDef.SearchParams param = new cSharpSkillBinding_TypeDef.SearchParams();
            param.search_plane = cSharpSkillBinding_TypeDef.SearchPlane.xy;
            param.hole_diameter = 0.0;
            param.search_range = 0.005;
            param.search_time = 1000;
            param.search_max_force = 10.0; 
            param.search_guide_force = 5;

            param.search_damp_scale = new cSharpSkillBinding_TypeDef.Vector6d(0.0, 0.0, 1000, 0.0, 0.0, 0.0);
            param.search_stiff_scale = new cSharpSkillBinding_TypeDef.Vector6d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);


            // 无参考轨迹
            param.guide_traj_type = cSharpSkillBinding_TypeDef.GuideTrajType.NONE;

            // 螺旋线
            //param.guide_traj_type = cSharpSkillBinding_TypeDef.GuideTrajType.SPIRAL;
            //param.spiral_param.step = 3;
            //param.spiral_param.direction = 1;
            //param.spiral_param.plane = cSharpSkillBinding_TypeDef.SpiralPlane.xy;
            //param.spiral_param.spiral = 0.005;
            //param.spiral_param.helix = 0.005;
            //param.spiral_param.radius = 0.06;

            // 摆线
            //param.guide_traj_type = cSharpSkillBinding_TypeDef.GuideTrajType.WEAVE;
            //param.weave_param.step = 30;
            //param.weave_param.direction = cSharpSkillBinding_TypeDef.WeaveSelect.rx;
            //param.weave_param.amplitude = 0.005;
            //param.weave_param.hold_distance = 0.2;
            //param.weave_param.angle = 0.1;

            cSharpSkillBinding_ForceControl.fcSearch(fc, param);

            // 等待接近流程结束
            int ret = cSharpSkillBinding_ForceControl.fcWaitCondition(fc);
            if (ret == (int)cSharpSkillBinding_TypeDef.StateCode.Search_Succeed)
            {
                Console.WriteLine("搜孔成功!");
            }
            else if (ret == (int)cSharpSkillBinding_TypeDef.StateCode.Search_MaxForce)
            {
                Console.Error.WriteLine("搜孔力过大!");
            }
            else if (ret == (int)cSharpSkillBinding_TypeDef.StateCode.TimeOut)
            {
                Console.Error.WriteLine("搜孔超时!");
            }
            else
            {
                Console.Error.WriteLine("Error state code!");
            }

            // 退出力控
            cSharpSkillBinding_ForceControl.fcExit(fc);
            cSharpSkillBinding_ForceControl.destroy_fc_ptr(fc);
        }

        static void Main(string[] args)
        {
            //初始化机械臂控制库
            IntPtr rpc_client = cSharpBinding_RPC.rpc_create_client(0);
            Console.Out.WriteLine("rpc_create_client ret={0}", rpc_client);
            if (rpc_client == IntPtr.Zero)
            {
                Console.Error.WriteLine("rpc_create_client failed!");
                return;
            }

            IntPtr rtde_client = cSharpBinding_RTDE.rtde_create_client(0);
            Console.Out.WriteLine("rtde_create_client ret={0}", rtde_client);
            if (rtde_client == IntPtr.Zero)
            {
                Console.Error.WriteLine("rtde_create_client failed!");
                return;
            }

            cSharpBinding_RPC.rpc_connect(rpc_client, robot_ip, server_port);
            cSharpBinding_RPC.rpc_setRequestTimeout(rpc_client, 1000);
            cSharpBinding_RPC.rpc_login(rpc_client, "aubo", "123456");

            cSharpBinding_RTDE.rtde_connect(rtde_client, robot_ip, server_port_1);
            cSharpBinding_RTDE.rtde_login(rtde_client, "aubo", "123456");

            if (exampleStartup(rpc_client) != 0)
            {
                cSharpBinding_RTDE.rtde_logout(rtde_client);
                cSharpBinding_RTDE.rtde_disconnect(rtde_client);
                cSharpBinding_RPC.rpc_logout(rpc_client);
                cSharpBinding_RPC.rpc_disconnect(rpc_client);
                return;
            }

            exampleInsert(rpc_client, rtde_client);
            //exampleSearch(rpc_client, rtde_client);

            cSharpBinding_RTDE.rtde_logout(rtde_client);
            cSharpBinding_RTDE.rtde_disconnect(rtde_client);

            cSharpBinding_RPC.rpc_logout(rpc_client);
            cSharpBinding_RPC.rpc_disconnect(rpc_client);
        }
    }
}
