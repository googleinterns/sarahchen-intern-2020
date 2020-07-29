import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DashboardService } from './dashboard.service'

@Injectable()
export class RequestHandler {
  constructor(private http: HttpClient) { }

  getDashboard(system: string) {
    return this.http.get<JSON>(DashboardService.API_ENDPOINT.concat(system));
  }
}