#include "main.h"
#include "daemon_controller.h"
#include <iostream>

namespace daemon_controller
{
    void handle_time_stamp(std::string description)
    {
        pthread_mutex_lock(&lockMsgHandlerSource);
        std::cout << "INFO: 处理 TIME_STAMP 信号 isMeasuring = " << PerfData.isMeasuring << std::endl;
        if (PerfData.isMeasuring < 0)
        {
            std::cout << "WARNING: measurement count < 0 (isMeasuring = " << PerfData.isMeasuring << ")" << std::endl;
        }
        else if (PerfData.isMeasuring == 0)
        {
            std::cout << "WARNING: measurement not started (isMeasuring = " << PerfData.isMeasuring << ")" << std::endl;
        }
        else
        {
            PerfData.vecAppStampDescription.emplace_back(description);
            std::cout << "INFO: save time stamp description (" << description << ")" << std::endl;
        }
        pthread_mutex_unlock(&lockMsgHandlerSource);
    }

    void handle_start()
    {
        pthread_mutex_lock(&lockMsgHandlerSource);
        if (PerfData.isMeasuring < 0)
        {
            pthread_mutex_unlock(&lockMsgHandlerSource);
            std::cout << "WARNING: measurement count < 0 (isMeasuring = " << PerfData.isMeasuring << ")" << std::endl;
        }
        else if (PerfData.isMeasuring == 0)
        {
            PerfData.isMeasuring++; // 启动计数++
            std::cout << "INFO: 处理 START 信号 isMeasuring = " << PerfData.isMeasuring << std::endl;
            pthread_mutex_unlock(&lockMsgHandlerSource);
            // 注册时钟信号，启动采样
            signal(SIGALRM, AlarmSampler);
            struct itimerval tick;
            tick.it_value.tv_sec = 0; // 0秒钟后将启动定时器
            tick.it_value.tv_usec = 1;
            tick.it_interval.tv_sec = 0; // 定时器启动后，每隔 Config.SampleInterval*1000 us 将执行相应的函数
            tick.it_interval.tv_usec = Config.SampleInterval * 1000;
            setitimer(ITIMER_REAL, &tick, NULL);
            // ualarm(10, Config.SampleInterval*1000);
            std::cout << "Sampling has already started." << std::endl;
            std::cout << "Sampling..." << std::endl;
        }
        // PerfData.isMeasuring++; // 启动计数++
        // std::cout << "INFO: 处理 START 信号 isMeasuring = " << PerfData.isMeasuring << std::endl;
        // pthread_mutex_unlock(&lockMsgHandlerSource);
    }

    void handle_stop()
    {
        usleep(0.2 * 1000 * 1000); // 暂停 0.2s ，使所有时间戳都被记录下来。
        std::cout << "INFO: 处理 STOP 信号 开始获得锁 lockMsgHandlerSource " << std::endl;
        pthread_mutex_lock(&lockMsgHandlerSource);
        std::cout << "INFO: 处理 STOP 信号 已经获得锁 lockMsgHandlerSource " << std::endl;
        PerfData.isMeasuring--; // 启动计数--
        std::cout << "INFO: 处理 STOP 信号 isMeasuring = " << PerfData.isMeasuring << std::endl;
        if (PerfData.isMeasuring < 0)
        {
            pthread_mutex_unlock(&lockMsgHandlerSource);
            std::cout << "WARNING: measurement count < 0 (isMeasuring = " << PerfData.isMeasuring << ")" << std::endl;
        }
        else if (PerfData.isMeasuring == 0)
        {
            pthread_mutex_unlock(&lockMsgHandlerSource);
            // 结束采样
            struct itimerval tick;
            getitimer(ITIMER_REAL, &tick); // 获得本次定时的剩余时间
            tick.it_interval.tv_sec = 0;   // 设置 interval 为0，完成本次定时之后，停止定时
            tick.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &tick, NULL); // 完成本次定时之后，停止定时
            // ualarm(0, Config.SampleInterval*1000);
            usleep(20 * 1000); // 暂停 20ms ，使所有时间戳都被记录下来。

            std::cout << "Sampling has already stopped." << std::endl;
            PerfData.output(Config);

            Config.Reset();
            PerfData.Reset();
        }
    }

    void handle_reset(std::string output_path)
    {
        unsigned int WaitNum = 10;
        for (unsigned int i = 0; i < WaitNum; i++)
        {
            pthread_mutex_lock(&lockMsgHandlerSource);
            std::cout << "INFO: 处理 RESET 信号 isMeasuring = " << PerfData.isMeasuring << std::endl;
            if (PerfData.isMeasuring > 0)
            {
                pthread_mutex_unlock(&lockMsgHandlerSource);
                if (i == (WaitNum - 1))
                {
                    std::cout << "WARNING: Sampling is still in progress! NOT RESET!" << std::endl;
                }
                else
                {
                    std::cout << "WARNING: Sampling is in progress! Wait 1s" << std::endl;
                    sleep(1); // 这里循环等待 1s * (WaitNum-1)，每秒监测一次
                }
                continue;
            }
            else
            {
                if (output_path.size() > 0)
                {
                    std::string OutPath = output_path;
                    Config.Reset(OutPath);
                    PerfData.Reset();
                    PerfData.SetOutPath(Config);
                }
                else
                {
                    Config.Reset();
                    PerfData.Reset();
                }

                pthread_mutex_unlock(&lockMsgHandlerSource);
                break;
            }
        }
    }

    void handle_reset_sm_clock()
    {
        PM.Reset();
    }

    void handle_exit()
    {
        int tmpMeasuring;
        for (unsigned int i = 0; i < 5; i++)
        {
            pthread_mutex_lock(&lockMsgHandlerSource);
            tmpMeasuring = PerfData.isMeasuring;
            pthread_mutex_unlock(&lockMsgHandlerSource);
            if (tmpMeasuring > 0)
            {
                std::cout << "WARNING: Sampling in progress!" << std::endl;
                usleep(5 * 1000 * Config.SampleInterval); // 这里循环等待 一共 5*5 个采样周期
            }
            else
            {
                break;
            }
        }
        if (tmpMeasuring > 0)
        {
            std::cout << "WARNING: Sampling in progress, but forced to exit!" << std::endl;
        }
        else
        {
            std::cout << "INFO: Measurement exit" << std::endl;
        }
    }

    HardwareStats handle_hardware_stats()
    {
        pthread_mutex_lock(&lockMsgHandlerSource);

        auto cpuPower = PerfData.currCPUPower;
        auto gpuPower = PerfData.currPower;

        auto cpuEnery = PerfData.CPUEnergy;
        auto gpuEnery = PerfData.Energy;

        auto memUsage = PerfData.currUtil.memory;
        auto gpuUsage = PerfData.currUtil.gpu;

        std::cout << "memUsage = " << memUsage << std::endl;

        pthread_mutex_unlock(&lockMsgHandlerSource);

        return {
            cpuPower = cpuPower,
            gpuPower = gpuPower,
            cpuEnery = cpuEnery,
            gpuEnery = gpuEnery,
            memUsage = memUsage,
            gpuUsage = gpuUsage,
        };
    }
} // namespace daemon_controller