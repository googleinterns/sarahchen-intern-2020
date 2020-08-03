import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { BACKEND_BASE_URL } from './constants'

@Injectable()
export class DashboardService {
  constructor(private http: HttpClient) { }

  getDashboard(system: string) {
    return this.http.get<JSON>(BACKEND_BASE_URL.concat(system));
  }
}