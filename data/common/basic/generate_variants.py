import os


for type in ["wall", "door", "ceiling", "ground"]:
    os.system(f"rm -rf {type}_variant")
    os.system(f"mkdir {type}_variant")
    for R in [0,1,2]:
        for G in [0,1,2]:
            for B in [0,1,2]:
                r = (R - 1.0) * 0.5
                g = (G - 1.0) * 0.5
                b = (B - 1.0) * 0.5
                print(f"{R}_{G}_{B}")
                os.system(f"convert  {type}.jpg -color-matrix \
                  \"1.0   0.0   0.0   0.0, 0.0,   {r}, \
                    0.0   1.0   0.0   0.0, 0.0,   {g}, \
                    0.0   0.0   1.0   0.0, 0.0,   {b}, \
                    0.0   0.0   0.0   1.0, 0.0,   0.0, \
                    0.0   0.0   0.0   0.0, 1.0,   0.0, \
                    0.0   0.0   0.0   0.0, 0.0,  1.0\" {type}_variant/{type}_{R}_{G}_{B}.jpg")
