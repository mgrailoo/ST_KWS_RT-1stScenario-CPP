# ST_KWS_RT-FirstScenario
Low-Power Keyword Spotting deployment using Voice Detection on Sensor-Tile Platform

Abstract—Keyword spotting (KWS) is a critical component in
low power smart devices with speech-based user interactions.
It is typically used as a front-end to wake up the full-scale
speech recognition. Since KWS still needs to be always on,
i.e. continuously listening, it has strong implications on power
consumption. In this paper, voice activity detection task is
activated using the build-in DFSDM analog watchdog to minimize
power consumption of KWS on SensorTile. The idea is putting
some parts of KWS in sleep mode when there is nothing of
interest to hear, or it is just noisy voice. The part awakes the
KWS as soon as voice is detected, while itself is always-on. It leads
to a reduction in the KWS power consumption. The impact of the
voice detection activation on power consumption is investigated
through three scenarios of normal, and low power deployment
with DMA streaming from beginning, or after the detection.
