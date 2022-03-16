# Note
### This consistently works for epee, but has problems with foil and saber.
This project currently detects the status of each fencer (i.e. which of the 6 lines, 3 from fencer A and 3 from fencer B, are in contact with each other) by pulling the 6 lines to various voltage levels and then reading the voltage levels on the lines. From these read voltage levels, we try to determine what combination of lines would result in that voltage level, and from that we determine what lines are connected. This was the first approach we thought of, but we learned that it is not robust (e.g. sensitive to variable resistances in cords, lames, etc), has issues with ambiguous configurations at certain voltage levels, and with the Arduinos we are unable to sample the lines fast enough to properly detect things like whipover in saber. We've thought of a better approach, but it is not feasible (at least for saber timings) on the Arduinos due to their limited clock speed.

The better approach we thought of would be to use a faster MCU (TI Launchpad seems promising), and then when we want to read configuration of the 6 lines we make most of them inputs except for one (or two, if one should be ground (e.g. strip, bell)), then see if we are reading any voltage on the inputs. By using analog inputs and a voltage divider, this also allows us to take resistence measurements in order to fully implement the rules exactly. I hope to one day make a v2 of this in the future, but have not done so at this time.  

# Inexfensive
### Overview
* We have built an easily reproducible, low cost device to monitor and scoring fencing matches.
* Equivalent commercial scoring systems are very expensive, and this prohibitive cost can prevent growth in the fencing community.
* By using easily accessible, low cost components, open-sourcing our code and providing building instructions, others can easily recreate our product. 
* Our ultimate goal is to allow other fencers or small fencing clubs to engage in electric fencing when they did not originally have the financial means.

### Benefits & Features
* Cost
  * Mid-range quality scoring devices sell for about $1000
  * This designs costs roughly $300
  * Users can choose to not include modules to save on cost and build time
* Reproducible and Accessible
  * Product is made of widely available components
  * Building instructions and code are available online
* Ability to Reprogram
  * This design can be easily updated after the inevitable rule changes
