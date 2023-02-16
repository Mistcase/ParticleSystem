
#include "test.h"

#include "./nvToolsExt.h"
#include "ParticleSystem.h"
#include <atomic>
#include <mutex>
#include <thread>

static uint8_t ps[sizeof(ParticleSystem)];
#define ps (*(ParticleSystem*)(ps))

static std::atomic_int globalTime;
static std::atomic_bool workerMustExit = false;

void WorkerThread(void)
{
    while (!workerMustExit)
    {
        nvtxRangePush(__FUNCTION__);

        static int lastTime = 0;
        const int time = globalTime.load();
        const int delta = time - lastTime;
        lastTime = time;

        if (delta > 0)
        {
            ps.update(delta / 1000.0f);
        }

        static const int MIN_UPDATE_PERIOD_MS = 10;
        if (delta < MIN_UPDATE_PERIOD_MS)
            std::this_thread::sleep_for(std::chrono::milliseconds(MIN_UPDATE_PERIOD_MS - delta));

        nvtxRangePop();
    }

    ps.~ParticleSystem();
}

void test::init(void)
{
    const Bounds bounds{ { 0.0f, 0.0f }, { test::SCREEN_WIDTH, test::SCREEN_HEIGHT } };
    new (&ps) ParticleSystem(bounds);

    std::thread workerThread(WorkerThread);
    workerThread.detach(); // Glut + MSVC = join hangs in atexit()
}

void test::term(void)
{
    workerMustExit = true;
}

void test::render(void)
{
    ps.render();
}

void test::update(int dt)
{
    globalTime.fetch_add(dt);
}

void test::on_click(int x, int y)
{
    ps.emit({ static_cast<float>(x), test::SCREEN_HEIGHT - static_cast<float>(y) });
}
