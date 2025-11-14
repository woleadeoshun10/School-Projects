---
# Topic 9
#  Routing Algorithms

## Overview
Routing algorithms determine the best path for data packets to travel from source to destination across a network.  
Each router maintains a **routing table** containing network addresses (CIDR) and their **Next Hop** destination.

---

## 🗺️ Example Routing Table

| **CIDR (Network)** | **Next Hop / Interface** |
|---------------------|---------------------------|
| 3.3.3.0/24      | Direct Connection       |
|4.4.4.0/24          | DC         |
| 5.5.5.0/24       | 4.4.4.2       |

- The router checks the **longest prefix match** to decide where to forward packets.  

---

##  ARP (Address Resolution Protocol)

- ARP maps **IP addresses → MAC addresses** within the same local network.  
- When a router needs to send a packet to a device, it must first know its **MAC address**.

**Example:**
1. Router wants to send to `192.168.1.5`
2. It broadcasts an ARP request:  
   _“Who has 192.168.1.5?”_
3. Device replies with its MAC address:  
   `00:1A:2B:3C:4D:5E`

**Result:** Router stores the mapping in its **ARP table** for faster future communication.

---

### Definitions
> **Router** is a host or network device that upon receiving a packet not intend for it, forwards it based in the best available information.
> **Network Operator** is an entity responsible for managing, operating, and maintaining a network (collection of hosts and routers, enforces a central authority)
>**Autonomous Sysytem (AS)** is a set of IP networks and routers under the control of a single Network Operator operating with a consistent and internally defined routing policy. Each AS is identified by a unique Autonomous System Number (ASN)
>**AS Path** is an order list of ASN that routing information has transversed to reach a particular network, used to prevent routing loops and determine the "best" path
>**BGP** Border Gateway Protocol is a EGP best path selection algorithm with a deterministic 12 step attributed based system for choosing the best path between multiple ASNs
>**iBGP (Internal Border Gateway Protocol)**
iBGP is used **within the same Autonomous System (AS)** to exchange routing information between routers that belong to that AS.  
- It helps maintain consistent BGP routing information across all routers inside one network.  
- Requires a **full mesh** of BGP sessions or the use of **Route Reflectors** to avoid routing loops.  
>**eBGP (External Border Gateway Protocol)**
eBGP is used **between different Autonomous Systems (ASes)** to exchange routing information across the internet.  
- Typically used by ISPs or organizations connecting to other networks.  
- Focuses on **policy-based routing** and **path selection** between administrative domains.  
>**Open Shoetest Path First(OSPF)** is a **link-state interior gateway protocol (IGP)** used to find the best path for routing IP packets within a single **autonomous system (AS)**.  
It uses the **Dijkstra Shortest Path First (SPF)** algorithm to compute the shortest and most efficient route.

---
| **Protocol Type**       | **Protocol Name(s)**              | **Focus**               | **Key Concept** |
|--------------------------|----------------------------------|--------------------------|-----------------|
| **External Gateway (EGP)** | **Border Gateway Protocol (BGP)** | **Policy-Based Routing** | Facilitates data exchange between two or more Autonomous Systems (AS), controlling routing decisions based on policies and network reachability. |
| **Internal Gateway (IGP)** | **OSPF**, **IS-IS**, **EIGRP**     | **Performance & Efficiency** | Optimizes routing within a single Autonomous System, focusing on speed, cost, and link-state awareness for best path selection. |

---
# ️ BGP Path Selection Process (12 Steps)

When multiple routes exist to the same destination, **BGP (Border Gateway Protocol)** applies a systematic set of decision-making steps to choose the *best path*.  
Each step is evaluated in order until one route is preferred.

---

##  The 12 Steps

1. **Next-Hop Reachability**  
   - Ensure the *next-hop IP address* is reachable in the routing table (RIB).  
   - If not reachable, the route is discarded.

2. **Highest Weight (Cisco Proprietary)**  
   - Local to the router — higher **weight** is preferred.  
   - Not advertised to other routers.  
   - Used for local outbound routing control.

3. **Highest Local Preference**  
   - Policy-driven attribute across the AS.  
   - Higher **local preference** (default = 100) wins.  
   - Used for inbound traffic engineering between multiple exit points.

4. **Locally Originated Routes**  
   - Prefer routes originated by the router itself (via **network** or **aggregate** commands).  
   - Ensures preference for *directly connected networks*.

5. **Shortest AS_PATH**  
   - Prefer routes that traverse the fewest autonomous systems.  
   - Reflects shortest interdomain distance.


---

##  Notes

- Steps **2** and **3** are **business-policy oriented**, used for traffic control and routing strategy.  

---

**In summary:**  
BGP path selection blends **business intent (policy control)** and **network optimization (technical efficiency)** to ensure reliable and predictable routing between autonomous systems.

---

## Shortest Path Algorithm – Dijkstra

- **Purpose:** Finds the **shortest path** from a source node to all other nodes in a weighted graph (used in routing protocols like OSPF).

###  Key Concepts
- **Algorithm Used:** Dijkstra’s Algorithm  
- **Core Operation:** *Relaxation* – continuously update the shortest known distance to each node.  
- **Data Structure:** Priority queue or min-heap to always pick the next closest node.  
- **Result:** A **Shortest Path Tree (SPT)** rooted at the source node.

### Process Steps
1. Start from the source node (distance = 0).  
2. Set all other nodes to ∞ (infinity).  
3. Pick the node with the smallest distance (unvisited).  
4. **Relaxation:**  
   - For each neighbor, check if `distance[source] + edge_weight < distance[neighbor]`.  
   - If yes, update the neighbor’s distance.  
5. Repeat until all nodes are visited.

### Used In
- **OSPF (Open Shortest Path First)** for link-state routing.

---
### 🔸 Routing in Real Networks
- Works on a **dynamic graph**:
  - **Links or nodes** can go offline (failures).  
  - **New links or nodes** can appear (scaling).  
  - **Costs (metrics)** like bandwidth, delay, or load can change.  
- Requires **distributed computation** — each router computes locally based on info from neighbors.  
- Must continuously **adapt** to topology changes.  
- Routing protocols exchange updates (e.g., OSPF LSAs, BGP advertisements).

---
###  LSA (Link State Advertisement)
- **Definition:** A packet type used in OSPF to exchange routing and topology information between routers.  
- Each router floods LSAs to its OSPF neighbors to describe its **links, costs, and connected networks**.  
- LSAs are sent to the **multicast address 224.0.0.5** (All OSPF Routers).  
  - 224.0.0.6 is used for **All Designated Routers (DR/BDR)**.

**Purpose:**  
Keep all routers in an area synchronized with the same view of the network.

---

###  LSDB (Link State Database)
- **Definition:** The database that stores all received LSAs within an OSPF area. It is used to calculate the routing table.
- Each router builds its **own LSDB**, but all routers in the same area have **identical LSDBs**.  
- From the LSDB, the router runs the **Dijkstra Shortest Path First (SPF)** algorithm to compute:
  - Shortest paths
  - Routing table entries 

---
###  When LSA Updates Occur:
1. **Link or Node Goes Offline**  
   - The router detects a failure and floods an updated LSA to all neighbors.

2. **New Link or Node Added**  
   - A new router or network interface is discovered, triggering new LSAs.

3. **Cost (Metric) Change**  
   - If a link’s bandwidth or delay changes, OSPF recalculates routes using the new cost.

4. **Receiving an LSA Update**  
   - When a router gets an updated LSA from a neighbor, it updates its LSDB and runs the **Dijkstra SPF algorithm** again to recompute the best paths.

---
## Incremental SPF (iSPF)

- **What it is:** An OSPF optimization that recomputes **only the affected parts** of the Shortest Path Tree after a topology change.
- **Why it matters:** Reduces CPU usage and speeds up convergence versus running a **full SPF** every time.
- **When it triggers:** Link/node up/down, cost changes, or new adjacencies (via LSA updates).


---
## OSPF Areas

-  **Definition:**  
  An **OSPF Area** is a logical grouping of routers that share the same link-state database to optimize routing efficiency.

- **Purpose:**  
  To reduce the size of the link-state database and limit the flooding of LSAs to within the area.

- **Key Concepts:**  
  - **Area 0 (Backbone Area):** All other areas must connect to this area.  
  - **Intra-Area Routing:** Routing within a single area.  
  - **Inter-Area Routing:** Routing between multiple OSPF areas through the backbone.

---
