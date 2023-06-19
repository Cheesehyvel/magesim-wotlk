import axios from "axios";

class Wcl {

    constructor(client_id) {
        this.base_url = "https://classic.warcraftlogs.com/api/v2/";
        this.client_id = client_id;
        this.oauth_token = null;
        this.loadAccessToken();
    }

    async getRaid(reportCode) {
        var raid = {
            fights: [],
            players: [],
        };

        var re = await this.query(`{
            reportData {
                report(code: "${reportCode}") {
                    fights(killType: Kills) {
                        id
                        encounterID
                        name
                        startTime
                        endTime
                        friendlyPlayers
                    }
                    playerDetails(startTime: 0, endTime: 9999999999)
                }
            }
        }`);

        raid.fights = _.get(re.data, "data.reportData.report.fights");
        if (raid.fights && _.isArray(raid.fights)) {
            var invalid = [
                744, // FL
                746, // Razorscale
                755, // Vezax
                756, // Yogg
                637, // Faction Champions
            ];
            raid.fights = raid.fights.filter(f => invalid.indexOf(f.encounterID) == -1);
        }

        raid.players = _.get(re.data, "data.reportData.report.playerDetails.data.playerDetails.dps");

        return raid;
    }

    async getFightData(reportCode, raid, fightID, playerID) {
        var data = {
            duration: 0,
            dp: [],
            timings: [],
            player: {
                gear: null,
            }
        };

        var fight = _.find(raid.fights, {id: fightID});
        data.duration = fight.endTime - fight.startTime;

        var re = await this.query(`{
            reportData {
                report(code: "${reportCode}") {
                    events(fightIDs: [${fight.id}], startTime: 0, endTime: 99999999999, dataType: Deaths, hostilityType: Friendlies) {
                        data
                    }
                }
            }
        }`);

        var chars = raid.players;
        var deaths = _.get(re.data, "data.reportData.report.events.data");
        if (!chars)
            throw new Error("Could not find any ranked characters");
        if (fight.friendlyPlayers)
            chars = chars.filter(c => fight.friendlyPlayers.indexOf(c.id) != -1);
        if (deaths) 
            chars = chars.filter(c => !_.find(deaths, {targetID: c.id}));
        var char = _.find(chars, {type: "Warrior"});
        if (!char)
            char = _.find(chars, {type: "Rogue"});
        if (!char)
            char = _.find(chars, {type: "DeathKnight"});
        if (!char)
            throw new Error("Could not find any warrior/rogue/dk");

        var re = await this.query(`{
            reportData {
                report(code: "${reportCode}") {
                    deaths: events(fightIDs: [${fight.id}], sourceID: ${char.id}, includeResources: true, hostilityType: Friendlies, dataType: All) {
                        data
                    }
                    gear: events(fightIDs: [${fight.id}], sourceID: ${playerID}, dataType: CombatantInfo, hostilityType: Friendlies) {
                        data
                    }
                }
            }
        }`);

        var sp = -1;
        var events = _.get(re.data, "data.reportData.report.deaths.data");
        if (events) {
            events.forEach(ev => {
                if (ev.type == "applybuff" && ev.hasOwnProperty("abilityGameID") && (ev.abilityGameID == 2825 || ev.abilityGameID == 32182)) {
                    data.timings.push({
                        name: "bloodlust",
                        t: ev.timestamp - fight.startTime,
                    });
                }
                if (ev.hasOwnProperty("spellPower") && ev.hasOwnProperty("armor") && ev.armor != 0 && ev.sourceID == char.id && ev.spellPower != sp) {
                    sp = ev.spellPower;
                    data.dp.push({
                        sp: ev.spellPower,
                        t: ev.timestamp - fight.startTime,
                    });
                }
            });

            if (data.dp.length > 1 && data.duration) {
                var total = 0;
                for (var i=0; i<data.dp.length; i++)
                    total+= data.dp[i].sp * ((i == data.dp.length-1 ? data.duration : data.dp[i+1].t) - data.dp[i].t);
                data.dp_avg = _.round(total/data.duration);
            }
        }

        data.player.gear = _.get(re.data, "data.reportData.report.gear.data.0.gear");

        return data;
    }

    async oauthInit(redir) {
        this.unsetAccessToken();

        var code_verifier = crypto.randomUUID()+crypto.randomUUID();
        localStorage.setItem("wcl_code_verifier", code_verifier);

        var challenge = await this.createChallenge(code_verifier);

        // var state = crypto.randomUUID();
        // localStorage.setItem("wcl_oauth_state", state);

        var params = new URLSearchParams({
            client_id: this.client_id,
            code_challenge: challenge,
            code_challenge_method: "S256",
            // state: state,
            redirect_uri: this.redirectUri(),
            response_type: "code",
        });

        var url = "https://classic.warcraftlogs.com/oauth/authorize?"+params.toString();
        if (redir) {
            window.location.href = url;
        }
        else {
            var width = Math.min(window.innerWidth-20, 720);
            var height = Math.min(window.innerHeight-20, 480);

            var options = [
                "popup=1",
                "width="+width,
                "height="+height,
                "left="+(window.innerWidth-width)/2,
                "top="+(window.innerHeight-height)/2,
            ];

            window.open(url,"WCL Login", options.join(","));
        }
    }

    async oauthConfirm(code) {
        var url = "https://classic.warcraftlogs.com/oauth/token";

        var code_verifier = localStorage.getItem("wcl_code_verifier");
        localStorage.removeItem("wcl_code_verifier");

        var data = {
            client_id: this.client_id,
            code_verifier: code_verifier,
            code: code,
            redirect_uri: this.redirectUri(),
            grant_type: "authorization_code",
        };
        data = new URLSearchParams(data);

        return new Promise((res, rej) => {
            axios.post(url, data)
            .then(r => {
                if (_.get(r.data, "access_token"))
                    res(r.data);
                else
                    rej("No access token in response");
            });
        });
    }

    hasAccessToken() {
        return this.access_token != null;
    }

    loadAccessToken(token) {
        var expires = localStorage.getItem("wcl_access_token_expires");
        if (expires) {
            var d = new Date;
            if (d.getTime() > expires)
                this.unsetAccessToken();
        }

        this.access_token = localStorage.getItem("wcl_access_token");
    }

    unsetAccessToken() {
        localStorage.removeItem("wcl_access_token");
        localStorage.removeItem("wcl_access_token_expires");
        this.access_token = null;
    }

    setAccessToken(data) {
        this.access_token = data.access_token;
        localStorage.setItem("wcl_access_token", data.access_token);

        var d = new Date;
        if (data.expires_in)
            d.setTime(d.getTime() + (data.expires_in - 7200)*1000); // Remove 2 days from expiration to be safe
        else
            d.setDate(d.getDate()+1); // 1 day expiration default
        localStorage.setItem("wcl_access_token_expires", d.getTime());
    }

    async query(q) {
        q = q.replace(/\s+/g, " ");

        return this.request("POST", "client", {
            query: q
        });
    }

    async request(method, uri, data) {
        if (!this.client_id) {
            return new Promise((res, rej) => {
                rej("Missing client id");
            });
        }

        if (!this.access_token) {
            this.oauthInit();

            return new Promise((res, rej) => {
                rej("oauth");
            });
        }

        var params = null;
        if (method.toLowerCase() == "get")
            params = data;

        return new Promise((res, rej) => {
            axios({
                method: method,
                url: this.base_url+uri,
                data: data,
                params: params,
                headers: {Authorization: "Bearer "+this.access_token}
            })
            .then(r => res(r))
            .catch(e => {
                if (e.status == 401 || e.status == 403)
                    this.oauthInit();
                else
                    rej(e);
            })
        });
    }

    redirectUri() {
        return window.location.origin+"/";
    }

    async createChallenge(code_verifier) {
        const encoder = new TextEncoder();
        const hash = await crypto.subtle.digest("SHA-256", encoder.encode(code_verifier));

        return btoa(String.fromCharCode.apply(null, new Uint8Array(hash)))
            .replace(/\+/g, "-")
            .replace(/\//g, "_")
            .replace(/=+$/, "");
    }

}

export { Wcl }