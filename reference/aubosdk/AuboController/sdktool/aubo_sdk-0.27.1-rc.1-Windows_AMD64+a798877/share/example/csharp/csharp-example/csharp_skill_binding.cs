using csharp_example;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using static csharp_skill_example.cSharpSkillBinding_TypeDef;

namespace csharp_skill_example
{

    internal class cSharpSkillBinding_TypeDef 
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Vector6d
        {
            public double X, Y, Z, Rx, Ry, Rz;

            public Vector6d(double x, double y, double z, double rx, double ry, double rz)
            {
                X = x;
                Y = y;
                Z = z;
                Rx = rx;
                Ry = ry;
                Rz = rz;
            }
        }

        public enum StateCode
        {
            // 失败状态码
            TimeOut = -100,         // 超时
            Search_MaxForce = -4,   // 搜孔达到最大力
            Insert_MaxForce = -3,   // 插入达到最大力
            Constant_NotTouch = -2, // 恒力过程中未接触
            Touch_Distance = -1,    // 超出探寻距离

            Running = 0, // 力控执行中

            // 成功状态码
            Touch_Succeed = 1,    // 接触成功
            Insert_Succeed = 2,   // 插孔成功
            Search_Succeed = 3,   // 搜孔成功
            Constant_Succeed = 4, // 接触成功

        }

        public enum GuideTrajType
        {
            NONE = 0,   // 无参考轨迹
            LINE1 = 1,  // 直线 speedLine
            LINE2 = 2,  // 直线 moveLine
            SPIRAL = 3, // 螺旋线
            WEAVE = 4,  // 摆线
        }

        // 螺旋线平面
        public enum SpiralPlane
        {
            xy = 0,
            yz = 1,
            zx = 2
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SpiralTrajParams
        {
            public double step;            // 圈数
            public double direction;       // 旋转方向 -1顺时针旋转，1逆时针旋转
            public SpiralPlane plane;      // 参考平面选择
            public double spiral;          // 螺旋线外扩
            public double helix;           // 螺旋上升 m
            public double radius;          // 第一圈半径 m
        }

        // 摆线方向
        public enum WeaveSelect
        {
            x = 1,
            y = 2,
            z = 3,
            rx = 4,
            ry = 5,
            rz = 6
        }

        // 摆线轨迹参数
        [StructLayout(LayoutKind.Sequential)]
        public struct WeaveTrajParams
        {
            public double step;                // 步长
            public double amplitude;           // 幅度 m
            public WeaveSelect direction;      // 方向
            public double hold_distance;       // 距离 m
            public double angle;               // 角度
            public double type;                // 类型 保留，目前只支持锯齿摆  
        }

        public enum InsertSelect
        {
            x = 1,
            y = 2,
            z = 3
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct InsertParams
        {
            public InsertSelect insert_select;     // 插入方向,可选x\y\z
            public double hole_diameter;           // 轴孔直径  m
            public double insert_max_speed;        // 插入速度限制 m/s
            public double insert_time;             // 插入时间限制 ms
            public Vector6d insert_max_force;      // 最大力限制
            public double insert_max_depth;        // 插入距离限制 m
            public double insert_guide_force;      // 插入引导力
            // 力控调节参数
            public Vector6d insert_damp_scale;     // 表示速度反应能力 [0-1] 值越大，反应越慢
            public Vector6d insert_stiff_scale;    // 表示系统软硬程序 [0-1] 值越大，越硬

            public GuideTrajType guide_traj_type;  // 主动轨迹类型
            public SpiralTrajParams spiral_param; // 螺旋轨迹参数
            public WeaveTrajParams weave_param;   // 摆动轨迹参数
        }

        // 搜孔平面
        public enum SearchPlane
        {
            yz = 1,
            xz = 2,
            xy = 3
        }


        // 搜孔参数
        [StructLayout(LayoutKind.Sequential)]
        public struct SearchParams
        {
            public SearchPlane search_plane;      // 搜孔平面
            public double hole_diameter;          // 轴孔直径 m， 保留
            public double search_range;           // 搜孔范围，保留
            public double search_time;            // 搜孔时间限制 ms
            public double search_max_force;       // 最大力限制
            public double search_guide_force;     // 搜孔引导力
             // 力控调节参数
            public Vector6d search_damp_scale;  // 表示速度反应能力 [0-1] 值越大，反应越慢
            public Vector6d search_stiff_scale; // 表示系统软硬程序 [0-1] 值越大，越硬

            public GuideTrajType guide_traj_type;   // 搜孔过程主动轨迹类型
            public SpiralTrajParams spiral_param;   // 螺旋轨迹参数
            public WeaveTrajParams weave_param;     // 摆动轨迹参数
        }

    }

    internal class cSharpSkillBinding_ForceControl
    {
        const string service_interface_dll = GlobalConstants.service_interface_dll;

        // 初始化力控工艺包对象
        [DllImport(service_interface_dll, EntryPoint = "create_fc_ptr", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr create_fc_ptr(IntPtr rpc, IntPtr rtde);

        // 反初始化力控工艺包对象
        [DllImport(service_interface_dll, EntryPoint = "destroy_fc_ptr", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void destroy_fc_ptr(IntPtr ptr);

        // 插孔功能函数
        [DllImport(service_interface_dll, EntryPoint = "fcInsert", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int fcInsert(IntPtr h,cSharpSkillBinding_TypeDef.InsertParams param);

        // 搜孔功能函数
        [DllImport(service_interface_dll, EntryPoint = "fcSearch", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int fcSearch(IntPtr h, cSharpSkillBinding_TypeDef.SearchParams param);

        // 等待力控结束，返回状态吗
        [DllImport(service_interface_dll, EntryPoint = "fcWaitCondition", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int fcWaitCondition(IntPtr h);

        // 退出力控
        [DllImport(service_interface_dll, EntryPoint = "fcExit", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int fcExit(IntPtr h);

        
    }
}
